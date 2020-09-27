// **************************************************************************
//
//       Copyright 1993-2001 NVIDIA, Corporation.  All rights reserved.
//
//     NOTICE TO USER:   The source code  is copyrighted under  U.S. and
//     international laws.  Users and possessors of this source code are
//     hereby granted a nonexclusive,  royalty-free copyright license to
//     use this code in individual and commercial software.
//
//     Any use of this source code must include,  in the user documenta-
//     tion and  internal comments to the code,  notices to the end user
//     as follows:
//
//     NVIDIA, CORPORATION MAKES NO REPRESENTATION ABOUT THE SUITABILITY
//     OF  THIS SOURCE  CODE  FOR ANY PURPOSE.  IT IS  PROVIDED  "AS IS"
//     WITHOUT EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORPOR-
//     ATION DISCLAIMS ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,
//     INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY, NONINFRINGE-
//     MENT,  AND FITNESS  FOR A PARTICULAR PURPOSE.   IN NO EVENT SHALL
//     NVIDIA, CORPORATION  BE LIABLE FOR ANY SPECIAL,  INDIRECT,  INCI-
//     DENTAL, OR CONSEQUENTIAL DAMAGES,  OR ANY DAMAGES  WHATSOEVER RE-
//     SULTING FROM LOSS OF USE,  DATA OR PROFITS,  WHETHER IN AN ACTION
//     OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,  ARISING OUT OF
//     OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOURCE CODE.
//
//     U.S. Government  End  Users.   This source code  is a "commercial
//     item,"  as that  term is  defined at  48 C.F.R. 2.101 (OCT 1995),
//     consisting  of "commercial  computer  software"  and  "commercial
//     computer  software  documentation,"  as such  terms  are  used in
//     48 C.F.R. 12.212 (SEPT 1995)  and is provided to the U.S. Govern-
//     ment only as  a commercial end item.   Consistent with  48 C.F.R.
//     12.212 and  48 C.F.R. 227.7202-1 through  227.7202-4 (JUNE 1995),
//     all U.S. Government End Users  acquire the source code  with only
//     those rights set forth herein.
//
// **************************************************************************
#include "CompileControl.h"
#if IS_WINNT5
#include <ntddvdeo.h>
#include <dmemmgr.h>
#endif
#include "nvtypes.h"

//***************************************************************************
// Include display driver IOCTL codes (shared by all components)
//***************************************************************************
#include <dspioctl.h>

#include "nvOverlay.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


#define GDI_CHANNEL_ID 0x00000000 // NOTE: assumes GDI is on channel 0

//***************************************************************************
// Special cased ROPS (In ROP3 single byte format)
//***************************************************************************

#define NV BLACKNESS 0x00
#define NV_DSTINVERT 0x55
#define NV_PATINVERT 0x5A
#define NV_SRCAND    0x88
#define NV_DSTCOPY   0xAA
#define NV_SRCCOPY   0xCC
#define NV_SRCPAINT  0xEE
#define NV_PATCOPY   0xF0
#define NV_WHITENESS 0xFF

// !!! CAUTION CAUTION !!!  See Note!!!
#define DMAPUSH_BUFFER_SIZE 0x200000    // NOTE: If you change these values, then
#define DMAPUSH_PADDING     32          //       change the EQUs in I386/STRUCS.INC
// !!! CAUTION CAUTION !!!              //


// these #defines are used by opengl api stereo 
#define OGL_STEREO_NUMBUFFERS       2
#define OGL_STEREO_BUFFER_LEFT      0
#define OGL_STEREO_BUFFER_RIGHT     1


#define MAX_INDEXED_IMAGE_NOTIFIERS 16       // Must be a power of 2
#define INDEXED_IMAGE_LUT_BUFFER_SIZE (MAX_INDEXED_IMAGE_NOTIFIERS * 256 * 4)
                                             // Allocate LUTs for each indexed image
                                             // object. These are associated so that
                                             // the LUT is not updated causing the
                                             // image to be corrupted
#define MAX_INDEXED_IMAGE_DWORDS 1792

#define MAX_CY_MEMORY_VALUE 0x7fff           // Hardware limitation

#ifndef NVD3D_DX6 // D3D driver conflicts
#define NV_IN_PROGRESS 255
#else
//
// DX6 Only:
// Redefine HAL CALLBACK function structs so they match the "NT" way.
//
//typedef DD_CALLBACKS       DDHAL_CALLBACKS;
typedef DD_SURFACECALLBACKS    DDHAL_SURFACECALLBACKS;
typedef DD_PALETTECALLBACKS    DDHAL_PALETTECALLBACKS;
#endif // #ifdef NVD3D_DX6


//***************************************************************************
// Number of cached cursors
//***************************************************************************

#define NUM_CACHED_CURSORS 4

//////////////////////////////////////////////////////////////////////
// Put all the conditional-compile constants here.  There had better
// not be many!

// Multi-board support can be enabled by setting this to 1:

#define MULTI_BOARDS            0

#if _WIN32_WINNT < 0x0500
//#define NT4_MULTI_DEV
#else
#undef NT4_MULTI_DEV
#endif

// This is the maximum number of boards we'll support in a single
// virtual driver:

#if MULTI_BOARDS
    #define MAX_BOARDS          16
    #define IBOARD(ppdev)       ((ppdev)->iBoard)
#else
    #define MAX_BOARDS          1
    #define IBOARD(ppdev)       0
#endif

//*********************************************************************************
// Number of dacs
// SAME one also define in miniport head file(s).  Search in miniport to match it.
//*********************************************************************************
#define NV_NO_DACS                  2
#define NV_NO_CONNECTORS                  24

// Useful for visualizing the offscreen heap:

#define DEBUG_HEAP              0

// Sorts out memory allocation macros
#ifndef PDEV_PTR
#define PDEV_PTR() ppdev
#endif

//*********************************************************************
// For NT4 for both NV3 and NV4, the display and DX drivers use the
// same version of the mem allocation macros.
//
// For Win2K, the NV4 DX6 driver has its own version of the memory
// allocation macros in D3DINC.H. For NV3, the Win2K DX driver is
// currently the same DX driver used for NT4, so it uses the following
// mem allocation in both the DX and display drivers. This will change
// once NV3 Win2K DX6 support is implemented.
//*********************************************************************

#if (!defined(NVD3D_DX6)) // [ && defined(NV4)) || (defined(NV3) && (defined(DISPDRV) ||  _WIN32_WINNT < 0x0500))


#define HEAP_ALLOC_DEPTH_WIDTH_HEIGHT        1
#define HEAP_ALLOC_SIZE                      2
#define HEAP_FREE                            3
#define HEAP_PURGE                           4
#define HEAP_INFO                            5
#define HEAP_ALLOC_TILED_PITCH_HEIGHT        6
#define HEAP_DESTROY                         7
#define TYPE_IMAGE                           0
#define TYPE_DEPTH                           1
#define TYPE_TEXTURE                         2
#define TYPE_OVERLAY                         3
#define TYPE_FONT                            4
#define TYPE_CURSOR                          5
#define TYPE_DMA                             6
#define TYPE_INSTANCE                        7
#define MEM_TYPE_PRIMARY                     8
#define MEM_TYPE_IMAGE_TILED                 9
#define TYPE_DEPTH_COMPR16                   10
#define TYPE_DEPTH_COMPR32                   11

#define NVHEAP_INFO()                           \
{                                               \
    NVOS11_PARAMETERS   HeapParams;             \
    PVOID pParms = (PVOID) &HeapParams;         \
    DWORD cbReturned;                           \
    HeapParams.hRoot = ppdev->hClient;          \
    HeapParams.hObjectParent = ppdev->hDevice;  \
    HeapParams.function = HEAP_INFO;            \
    HeapParams.owner    = 'NVDD';               \
    EngDeviceIoControl(                         \
        ppdev->hDriver,                         \
        (DWORD)IOCTL_NV01_ARCH_HEAP,            \
        (&pParms),                              \
        sizeof(PVOID),                          \
        pParms,                                 \
        sizeof(NVOS11_PARAMETERS),              \
        &cbReturned                             \
    );                                          \
    ppdev->VideoHeapTotal = HeapParams.total;   \
    ppdev->VideoHeapFree = HeapParams.free;     \
}

#define NVHEAP_PURGE()                          \
{                                               \
    NVOS11_PARAMETERS HeapParams;               \
    PVOID pParms = (PVOID) &HeapParams;         \
    DWORD cbReturned;                           \
    HeapParams.hRoot = ppdev->hClient;          \
    HeapParams.hObjectParent = ppdev->hDevice;  \
    HeapParams.function = HEAP_PURGE;           \
    HeapParams.owner    = 'NVDD';               \
    EngDeviceIoControl(                         \
        ppdev->hDriver,                         \
        (DWORD)IOCTL_NV01_ARCH_HEAP,            \
        (&pParms),                              \
        sizeof(PVOID),                          \
        pParms,                                 \
        sizeof(NVOS11_PARAMETERS),              \
        &cbReturned                             \
    );                                          \
}

#define NVHEAP_DESTROY()                        \
{                                               \
    NVOS11_PARAMETERS HeapParams;               \
    PVOID pParms = (PVOID) &HeapParams;         \
    DWORD cbReturned;                           \
    HeapParams.hRoot = ppdev->hClient;          \
    HeapParams.hObjectParent = ppdev->hDevice;  \
    HeapParams.function = HEAP_DESTROY;         \
    HeapParams.owner    = 'NVDD';               \
    EngDeviceIoControl(                         \
        ppdev->hDriver,                         \
        (DWORD)IOCTL_NV01_ARCH_HEAP,            \
        (&pParms),                              \
        sizeof(PVOID),                          \
        pParms,                                 \
        sizeof(NVOS11_PARAMETERS),              \
        &cbReturned                             \
    );                                          \
}

#define NVHEAP_INIT(strt, end, heap) \
{                                    \
    PDEV_PTR()->cbGdiHeap = 0;          \
}

#define NVHEAP_CLEAR(heap)  \
{                           \
    PDEV_PTR()->cbGdiHeap = 0; \
}

#define NVHEAP_ALLOC(sts,pvm,sz,tp)                     \
{                                                       \
    NVOS11_PARAMETERS   HeapParams;                     \
    PVOID pParms = (PVOID) &HeapParams;                 \
    DWORD cbReturned;                                   \
                                                        \
    HeapParams.hRoot = PDEV_PTR()->hClient;                \
    HeapParams.hObjectParent = PDEV_PTR()->hDevice;        \
    HeapParams.function = HEAP_ALLOC_SIZE;              \
    HeapParams.owner    = 'NVDD';                       \
    HeapParams.type     = (tp);                         \
    HeapParams.size     = (sz);                         \
    EngDeviceIoControl(                                 \
        PDEV_PTR()->hDriver,                               \
        (DWORD)IOCTL_NV01_ARCH_HEAP,                    \
        (&pParms),                                      \
        sizeof(PVOID),                                  \
        pParms,                                         \
        sizeof(NVOS11_PARAMETERS),                      \
        &cbReturned                                     \
    );                                                  \
    (sts) = HeapParams.status;                          \
    (pvm) = (HeapParams.status ? 0 : HeapParams.offset);\
    if (pvm)                                            \
        {                                               \
        PDEV_PTR()->cbGdiHeap += PDEV_PTR()->VideoHeapFree -  \
            HeapParams.free;                            \
        PDEV_PTR()->VideoHeapFree = HeapParams.free;       \
        }                                               \
}

#define NVHEAP_ALLOC_TILED(sts,pvm,pch,ht,tp)           \
{                                                       \
    NVOS11_PARAMETERS HeapParams;                       \
    PVOID pParms = (PVOID) &HeapParams;                 \
    DWORD cbReturned;                                   \
    HeapParams.hRoot = PDEV_PTR()->hClient;                \
    HeapParams.hObjectParent = PDEV_PTR()->hDevice;        \
    HeapParams.function = HEAP_ALLOC_TILED_PITCH_HEIGHT;\
    HeapParams.owner    = 'NVDD';                       \
    HeapParams.type     = (tp);                         \
    HeapParams.pitch    = (pch);                        \
    HeapParams.height   = (ht);                         \
    EngDeviceIoControl(                                 \
        PDEV_PTR()->hDriver,                               \
        (DWORD)IOCTL_NV01_ARCH_HEAP,                    \
        (&pParms),                                      \
        sizeof(PVOID),                                  \
        pParms,                                         \
        sizeof(NVOS11_PARAMETERS),                      \
        &cbReturned                                     \
    );                                                  \
    (sts) = HeapParams.status;                          \
    (pvm) = HeapParams.status ? 0 : HeapParams.offset;  \
    (pch) = HeapParams.pitch;                           \
    if (pvm)                                            \
        {                                               \
        PDEV_PTR()->cbGdiHeap += PDEV_PTR()->VideoHeapFree -  \
            HeapParams.free;                            \
        PDEV_PTR()->VideoHeapFree = HeapParams.free;       \
        }                                               \
}

#define NVHEAP_FREE(pvm)                        \
{                                               \
    NVOS11_PARAMETERS HeapParams;               \
    PVOID pParms = (PVOID) &HeapParams;         \
    DWORD cbReturned;                           \
    HeapParams.hRoot = PDEV_PTR()->hClient;        \
    HeapParams.hObjectParent = PDEV_PTR()->hDevice;\
    HeapParams.function = HEAP_FREE;            \
    HeapParams.owner    = 'NVDD';               \
    HeapParams.offset   = (U032)(pvm);          \
    EngDeviceIoControl(                         \
        PDEV_PTR()->hDriver,                       \
        (DWORD)IOCTL_NV01_ARCH_HEAP,            \
        (&pParms),                              \
        sizeof(PVOID),                          \
        pParms,                                 \
        sizeof(NVOS11_PARAMETERS),              \
        &cbReturned                             \
    );                                          \
    PDEV_PTR()->cbGdiHeap -= (HeapParams.free -    \
        PDEV_PTR()->VideoHeapFree);                \
    PDEV_PTR()->VideoHeapFree = HeapParams.free;   \
}

#endif // (!defined(NVD3D_DX6)) ]

//////////////////////////////////////////////////////////////////////
// Miscellaneous shared stuff

#define DLL_NAME                L"NV_DISP"  // Name of the DLL in UNICODE
#define STANDARD_DEBUG_PREFIX   "NV: "      // All debug output is prefixed
                                            //   by this string
#define ALLOC_TAG               '  VN'      // Four byte tag used for tracking
                                            //   memory allocations (characters
                                            //   are in reverse order)

#define CLIP_LIMIT          50  // We'll be taking 800 bytes of stack space

#define DRIVER_EXTRA_SIZE   0   // Size of the DriverExtra information in the
                                //   DEVMODE structure

#define MEM_TO_MEM_BUFFER_SIZE    (0x80000)  // 512kb
 
                                  // Size in bytes of 'MEM_TO_MEM_Buffer'.  Has to
                                  //   be at least enough to store an entire
                                  //   scan line (i.e., 6400 for 1600x1200x32).

#if defined(_ALPHA_)
    #define XFER_BUFFERS    16  // Defines the maximum number of write buffers
                                //   possible on any Alpha.  Must be a power
#else                           //   of two.
    #define XFER_BUFFERS    1   // On non-alpha systems, we don't have to
                                //   worry about the chip caching our bus
#endif                          //   writes.

#define XFER_MASK           (XFER_BUFFERS - 1)

typedef struct _CLIPENUM {
    LONG    c;
    RECTL   arcl[CLIP_LIMIT];   // Space for enumerating complex clipping

} CLIPENUM;                         /* ce, pce */


// structure to handle FC_RECT4 but doesn't use so much stack space as CLIPENUM
typedef struct _ENUMRECT4 {
    LONG    c;
    RECTL   arcl[4];   // Space for enumerating FC_RECT4 clipping
} ENUMRECT4;


typedef struct _NV_VIDEO_CLUTDATA {
    UCHAR Blue;
    UCHAR Green;
    UCHAR Red;
    UCHAR Unused;
} NV_VIDEO_CLUTDATA, *PNV_VIDEO_CLUTDATA;

// BUGBUG - Hack 'o rama, these should be NvNotification structs!
typedef struct _NV_NOTIFIERS
{
    BYTE   VideoLutCursorDac[16*9];
    BYTE   Sync[16*5];
    BYTE   FlipPrimary[16*9];
    BYTE   FlipOverlay[16*5];
    BYTE   FlipVport[16*5];
    BYTE   DmaFromMem[16*5];
    BYTE   DmaToMem[16*5];
    BYTE   DmaToMemIndexedImage[16*MAX_INDEXED_IMAGE_NOTIFIERS];
    BYTE   MemToMem[16*2];
    BYTE   ScaledImage[16*5];
    BYTE   Primary2d[16*5];
    BYTE   DmaPushBufErr[16];
} NV_NOTIFIERS, *PNV_NOTIFIERS;

typedef struct _PDEV PDEV;      // Handy forward declaration

typedef struct _GLOBALDATA GLOBALDATA; // Defined in DD driver's DDMINI.H (NT5)
                                       // or Display Driver's DDMININT.H (NT4)


VOID vSetClipping(PDEV*, RECTL*);
VOID vResetClipping(PDEV*);

//////////////////////////////////////////////////////////////////////
// Text stuff

#define GLYPH_CACHE_HEIGHT  48  // Number of scans to allocate for glyph cache,
                                //   divided by pel size

#define GLYPH_CACHE_CX      64  // Maximal width of glyphs that we'll consider
                                //   caching

#define GLYPH_CACHE_CY      64  // Maximum height of glyphs that we'll consider
                                //   caching

#define MAX_GLYPH_SIZE      ((GLYPH_CACHE_CX * GLYPH_CACHE_CY + 31) / 8)
                                // Maximum amount of off-screen memory required
                                //   to cache a glyph, in bytes

#define GLYPH_ALLOC_SIZE    8100
                                // Do all cached glyph memory allocations
                                //   in 8k chunks

#define HGLYPH_SENTINEL     ((ULONG) -1)
                                // GDI will never give us a glyph with a
                                //   handle value of 0xffffffff, so we can
                                //   use this as a sentinel for the end of
                                //   our linked lists

#define GLYPH_HASH_SIZE     256

#define GLYPH_HASH_FUNC(x)  ((x) & (GLYPH_HASH_SIZE - 1))

typedef struct _CACHEDGLYPH CACHEDGLYPH;
typedef struct _CACHEDGLYPH
{
    CACHEDGLYPH*    pcgNext;    // Points to next glyph that was assigned
                                //   to the same hash table bucket
    HGLYPH          hg;         // Handles in the bucket-list are kept in
                                //   increasing order
    POINTL          ptlOrigin;  // Origin of glyph bits

    // Device specific fields below here:

    LONG            cxLessOne;  // Glyph width less one
    LONG            cyLessOne;  // Glyph height less one
    LONG            cxcyLessOne;// Packed width and height, less one
    LONG            cw;         // Number of words to be transferred
    LONG            cd;         // Number of dwords to be transferred
    ULONG           ad[1];      // Start of glyph bits
} CACHEDGLYPH;  /* cg, pcg */

typedef struct _GLYPHALLOC GLYPHALLOC;
typedef struct _GLYPHALLOC
{
    GLYPHALLOC*     pgaNext;    // Points to next glyph structure that
                                //   was allocated for this font
    CACHEDGLYPH     acg[1];     // This array is a bit misleading, because
                                //   the CACHEDGLYPH structures are actually
                                //   variable sized
} GLYPHAALLOC;  /* ga, pga */

typedef struct _CACHEDFONT CACHEDFONT;
typedef struct _CACHEDFONT
{
    CACHEDFONT*     pcfNext;    // Points to next entry in CACHEDFONT list
    CACHEDFONT*     pcfPrev;    // Points to previous entry in CACHEDFONT list
    GLYPHALLOC*     pgaChain;   // Points to start of allocated memory list
    CACHEDGLYPH*    pcgNew;     // Points to where in the current glyph
                                //   allocation structure a new glyph should
                                //   be placed
    LONG            cjAlloc;    // Bytes remaining in current glyph allocation
                                //   structure
    CACHEDGLYPH     cgSentinel; // Sentinel entry of the end of our bucket
                                //   lists, with a handle of HGLYPH_SENTINEL
    CACHEDGLYPH*    apcg[GLYPH_HASH_SIZE];
                                // Hash table for glyphs

} CACHEDFONT;   /* cf, pcf */

typedef struct _XLATECOLORS {       // Specifies foreground and background
ULONG   iBackColor;                 //   colours for faking a 1bpp XLATEOBJ
ULONG   iForeColor;
} XLATECOLORS;                          /* xlc, pxlc */


//********************************************************************
// This cursor structure MUST also match the one defined in nv.h
// (in the miniport directory)
//********************************************************************

typedef struct _TV_CURSOR_ADJUST_INFO
    {
    ULONG   MonitorType;
    ULONG   Underscan_x;
    ULONG   Underscan_y;
    ULONG   Scale_x;
    ULONG   Scale_y;
    ULONG   FilterEnable;
    ULONG   TVCursorMin;
    ULONG   TVCursorMax;
    } TV_CURSOR_ADJUST_INFO;


BOOL bEnableText(PDEV*);
VOID vDisableText(PDEV*);
VOID vAssertModeText(PDEV*, BOOL);

VOID vFastText(GLYPHPOS*, ULONG, BYTE*, ULONG, ULONG, RECTL*, RECTL*,
               FLONG, RECTL*, RECTL*);
VOID vClearMemDword(ULONG*, ULONG);

//////////////////////////////////////////////////////////////////////
// Dither stuff

// Describes a single colour tetrahedron vertex for dithering:

typedef struct _VERTEX_DATA {
    ULONG ulCount;              // Number of pixels in this vertex
    ULONG ulVertex;             // Vertex number
} VERTEX_DATA;                      /* vd, pv */

VERTEX_DATA*    vComputeSubspaces(ULONG, VERTEX_DATA*);
VOID            vDitherColor(ULONG*, VERTEX_DATA*, VERTEX_DATA*, ULONG);

//////////////////////////////////////////////////////////////////////
// Brush stuff

// 'Slow' brushes are used when we don't have hardware pattern capability,
// and we have to handle patterns using screen-to-screen blts:

#define SLOW_BRUSH_CACHE_DIM    3   // Controls the number of brushes cached
                                    //   in off-screen memory, when we don't
                                    //   have the S3 hardware pattern support.
                                    //   We allocate 3 x 3 brushes, so we can
                                    //   cache a total of 9 brushes:
#define SLOW_BRUSH_COUNT        (SLOW_BRUSH_CACHE_DIM * SLOW_BRUSH_CACHE_DIM)
#define SLOW_BRUSH_DIMENSION    64  // After alignment is taken care of,
                                    //   every off-screen brush cache entry
                                    //   will be 64 pels in both dimensions
#define SLOW_BRUSH_ALLOCATION   (SLOW_BRUSH_DIMENSION + 8)
                                    // Actually allocate 72x72 pels for each
                                    //   pattern, using the 8 extra for brush
                                    //   alignment

// 'Fast' brushes are used when we have hardware pattern capability:

#define FAST_BRUSH_COUNT        16  // Total number of non-hardware brushes
                                    //   cached off-screen
#define FAST_BRUSH_DIMENSION    8   // Every off-screen brush cache entry
                                    //   is 8 pels in both dimensions
#define FAST_BRUSH_ALLOCATION   8   // We have to align ourselves, so this is
                                    //   the dimension of each brush allocation

// Common to both implementations:

#define RBRUSH_2COLOR           1   // For RBRUSH flags

#define TOTAL_BRUSH_COUNT       max(FAST_BRUSH_COUNT, SLOW_BRUSH_COUNT)
                                    // This is the maximum number of brushes
                                    //   we can possibly have cached off-screen
#define TOTAL_BRUSH_SIZE        64  // We'll only ever handle 8x8 patterns,
                                    //   and this is the number of pels

typedef struct _BRUSHENTRY BRUSHENTRY;

// NOTE: Changes to the RBRUSH or BRUSHENTRY structures must be reflected
//       in strucs.inc!

typedef struct _RBRUSH {
    FLONG       fl;             // Type flags
    BOOL        bTransparent;   // TRUE if brush was realized for a transparent
                                //   blt (meaning colours are white and black),
                                //   FALSE if not (meaning it's already been
                                //   colour-expanded to the correct colours).
                                //   Value is undefined if the brush isn't
                                //   2 colour.
    ULONG       ulForeColor;    // Foreground colour if 1bpp
    ULONG       ulBackColor;    // Background colour if 1bpp
    POINTL      ptlBrushOrg;    // Brush origin of cached pattern.  Initial
                                //   value should be -1
    BRUSHENTRY* apbe[MAX_BOARDS];// Points to brush-entry that keeps track
                                //   of the cached off-screen brush bits
    ULONG       aulPattern[1];  // Open-ended array for keeping copy of the
      // Don't put anything     //   actual pattern bits in case the brush
      //   after here, or       //   origin changes, or someone else steals
      //   you'll be sorry!     //   our brush entry (declared as a ULONG
                                //   for proper dword alignment)

} RBRUSH;                           /* rb, prb */

typedef struct _BRUSHENTRY {
    RBRUSH*     prbVerify;      // We never dereference this pointer to
                                //   find a brush realization; it is only
                                //   ever used in a compare to verify
                                //   that for a given realized brush, our
                                //   off-screen brush entry is still valid.
    LONG        x;              // x-position of cached pattern
    LONG        y;              // y-position of cached pattern

} BRUSHENTRY;                       /* be, pbe */

typedef union _RBRUSH_COLOR {
    RBRUSH*     prb;
    ULONG       iSolidColor;
} RBRUSH_COLOR;                     /* rbc, prbc */


//////////////////////////////////////////////////////////////////////
// Stretch stuff

typedef struct _STR_BLT {
    PDEV*   ppdev;
    PBYTE   pjSrcScan;
    LONG    lDeltaSrc;
    LONG    XSrcStart;
    PBYTE   pjDstScan;
    LONG    lDeltaDst;
    LONG    XDstStart;
    LONG    XDstEnd;
    LONG    YDstStart;
    LONG    YDstCount;
    ULONG   ulXDstToSrcIntCeil;
    ULONG   ulXDstToSrcFracCeil;
    ULONG   ulYDstToSrcIntCeil;
    ULONG   ulYDstToSrcFracCeil;
    ULONG   ulXFracAccumulator;
    ULONG   ulYFracAccumulator;
} STR_BLT;

typedef VOID (*PFN_DIRSTRETCH)(STR_BLT*);

VOID vDirectStretch8Narrow(STR_BLT*);
VOID vDirectStretch8(STR_BLT*);
VOID vDirectStretch16(STR_BLT*);
VOID vDirectStretch32(STR_BLT*);

/////////////////////////////////////////////////////////////////////////
// Heap stuff

typedef struct _DSURF DSURF;
typedef struct _HEAP HEAP;
typedef struct _OH OH;

typedef struct _OH
{
    DSURF*  pdsurf;         // corresponding DSURF structure
    OH*     next;           // for 2-link list
    OH*     prev;           // for 2-link list
    OH*     nextEmpty;      // for 1-link list of empty fragments
    HEAP*   pHeap;          // pointer to heap containing this fragment
                            // not used if using RM_MMGR exclusively

    ULONG   ulSizeInBytes;  // size in bytes of fragment
    ULONG   ulOffset;       // FB offset to allocation
    ULONG   ulLinearStride; // stride in bytes (that was used to enforce alignment restrictions)
#ifdef _WIN64
    ULONG   pad;            // keep struct 64bit aligned for IA64 builds
#endif
}
OH;

typedef struct _HEAP
{
    OH      oh;             // Contains base OH struct for entire heap.

    HEAP*   pNextHeap;
    HEAP*   pPrevHeap;

    ULONG   ulSizeInBytes;  // size in bytes of this heap
    ULONG   ulOffset;       // physical offset to base of heap
}
HEAP;

typedef enum {
    DT_SCREEN,              // Surface is kept in screen memory
    DT_DIB                  // Surface is kept as a DIB
} DSURFTYPE;                    /* dt, pdt */

typedef struct _DSURF
{
    DSURFTYPE dt;           // DSURF status (whether off-screen or in a DIB)
    ULONG     pad;          // keep pointers below 64bit aligned for IA64

    SIZEL     sizl;         // Size of the original bitmap (could be smaller
                            //   than poh->sizl)
    PDEV*     ppdev;        // Need this for deleting the bitmap
    union {
        OH*         poh;    // If DT_SCREEN, points to off-screen heap node
        SURFOBJ*    pso;    // If DT_DIB, points to locked GDI surface
    };

    DSURF*    next;         // for 2-link list of DSURFs
    DSURF*    prev;         // for 2-link list of DSURFs

    // The following are used for DT_DIB only...

    ULONG     cBlt;         // Counts down the number of blts necessary at
                            //   the current uniqueness before we'll consider
                            //   putting the DIB back into off-screen memory
    ULONG     iUniq;        // Tells us whether there have been any heap
                            //   'free's since the last time we looked at
                            //   this DIB

    /* the following information is from OH structure */
    VOID*    LinearPtr;
    ULONG    LinearStride;

} DSURF;                          /* dsurf, pdsurf */

// GDI expects dword alignment for any bitmaps on which it is expected
// to draw.  Since we occasionally ask GDI to draw directly on our off-
// screen bitmaps, this means that any off-screen bitmaps must be dword
// aligned in the frame buffer.  We enforce this merely by ensuring that
// all off-screen bitmaps are four-pel aligned (we may waste a couple of
// pixels at the higher colour depths):

#define HEAP_X_ALIGNMENT    4

// Number of blts necessary before we'll consider putting a DIB DFB back
// into off-screen memory:

#define HEAP_COUNT_DOWN     6

// Flags for 'pohAllocate':

typedef enum {
    FLOH_ONLY_IF_ROOM       = 0x0001,   // Don't kick stuff out of off-
                                        //   screen memory to make room
    FLOH_MAKE_PERMANENT     = 0x0002,   // Allocate a permanent entry
    FLOH_RESERVE            = 0x0004,   // Allocate an off-screen entry,
                                        //   but let it be used by discardable
                                        //   bitmaps until it's needed
} FLOH;

// Publicly callable heap APIs:

OH*  pohAllocate(PDEV*, LONG, LONG);
void pohFree(PDEV*, OH*);

BOOL pohMoveOffscreenDfbToDib(PDEV*, OH*);
BOOL bMoveDibToOffscreenDfbIfRoom(PDEV*, DSURF*);
BOOL __fastcall bMoveAllDfbsFromOffscreenToDibs(PDEV* ppdev);

BOOL bEnableOffscreenHeap(PDEV*);
VOID vDisableOffscreenHeap(PDEV*);
BOOL bAssertModeOffscreenHeap(PDEV*, BOOL);


/////////////////////////////////////////////////////////////////////////
// Pointer stuff

#define POINTER_DATA_SIZE       40      // Number of bytes to allocate for the
                                        //   miniport down-loaded pointer code
                                        //   working space
#define HW_INVISIBLE_OFFSET     2       // Offset from 'ppdev->yPointerBuffer'
                                        //   to the invisible pointer
//#define HW_POINTER_DIMENSION    64      // Maximum dimension of default
//                                        //   (built-in) hardware pointer
#define HW_POINTER_DIMENSION    32      // Maximum dimension of default
                                        //   (built-in) hardware pointer


#define HW_POINTER_HIDE         63      // Hardware pointer start pixel
                                        //   position used to hide the pointer
//#define HW_POINTER_TOTAL_SIZE   1024    // Total size in bytes required
//                                        //   to define the hardware pointer
//                                        //   (must be a power of 2 for
//                                        //   allocating space for the shape)
#define NV1_POINTER_TOTAL_SIZE   256    // Total size in bytes required



typedef VOID (FNSHOWPOINTER)(PDEV*, BOOL);
typedef VOID (FNMOVEPOINTER)(PDEV*, LONG, LONG);
typedef ULONG (FNSETPOINTERSHAPE)(SURFOBJ *,SURFOBJ*,SURFOBJ*,XLATEOBJ*,LONG,LONG,LONG,LONG,LONG,LONG,BOOL);
typedef VOID (FNENABLEPOINTER)(PDEV*, BOOL);

BOOL bEnablePointer(PDEV*);
VOID vDisablePointer(PDEV*);
VOID vAssertModePointer(PDEV*, BOOL);

/////////////////////////////////////////////////////////////////////////
// Palette stuff

BOOL bEnablePalette(PDEV*);
VOID vDisablePalette(PDEV*);
VOID vAssertModePalette(PDEV*, BOOL);

BOOL bInitializePalette(PDEV*, DEVINFO*);
VOID vUninitializePalette(PDEV*);
ULONG nvSetGammaRamp(PDEV *ppdev, PUSHORT fpRampVals);
ULONG nvGetGammaRamp(PDEV *ppdev, PUSHORT fpRampVals);
BOOL nvSetHeadGammaRamp(PDEV *ppdev, ULONG ulHead, PUSHORT fpRampVals);
BOOL nvGetHeadGammaRamp(PDEV *ppdev, ULONG ulHead, PUSHORT fpRampVals);

#if 0 // _WIN32_WINNT >= 0x0500
BOOL    DrvIcmSetDeviceGammaRamp(DHPDEV, ULONG, LPVOID);
HBITMAP DrvDeriveSurface(DD_DIRECTDRAW_GLOBAL *, DD_SURFACE_LOCAL *);
#endif

#define MAX_CLUT_SIZE (sizeof(VIDEO_CLUT) + (sizeof(ULONG) * 256))
#define ROUND_UP_TO_64K(x)  (((ULONG)(x) + 0x10000 - 1) & ~(0x10000 - 1))

#ifndef NVD3D
/////////////////////////////////////////////////////////////////////////
// DirectDraw stuff

// There's a 64K granularity that applies to the mapping of the frame
// buffer into the application's address space:
#endif // #ifndef NVD3D

BOOL bEnableDirectDraw(PDEV*);
VOID vDisableDirectDraw(PDEV*);
BOOL bAssertModeDirectDraw(PDEV*, BOOL);

#ifdef VPE_NT
DWORD   VPE_DriverInit(PDEV*);
#endif

//////////////////////////////////////////////////////////////////////
// Low-level blt function prototypes

#if (_WIN32_WINNT < 0x0500)
#define BLENDOBJ   VOID
#endif


typedef VOID (FNSETDESTBASE)(PDEV*, ULONG, LONG);
typedef VOID (FNSETSOURCEBASE)(PDEV*, ULONG, LONG);

typedef VOID (FNFILL)(PDEV*, LONG, RECTL*, ULONG, RBRUSH_COLOR, POINTL*);
typedef VOID (FNMEMTOSCRN)(PDEV*, SURFOBJ*, POINTL*, RECTL *);
typedef BOOL (FNSCRNTO1BPPMEM)(PDEV*, LONG, RECTL *, SURFOBJ*, SURFOBJ*, POINTL*, RECTL *, XLATEOBJ *);

typedef BOOL (FNSCRNTOMEM)(PDEV*, LONG, RECTL *, SURFOBJ*, SURFOBJ*, POINTL*, RECTL *, XLATEOBJ *);
typedef VOID (FNSCRNTOSCRNPAT)(PDEV*, LONG, RECTL*, POINTL*, RECTL*, BRUSHOBJ*, POINTL*, ROP4);

typedef VOID (FNGETSCRNBITS)(PDEV* ,DSURF* ,SURFOBJ* ,RECTL* ,POINTL* );

typedef VOID (FNMEMTOSCRNPATTERN)(PDEV*, SURFOBJ*, POINTL*, RECTL *,ULONG ,RBRUSH_COLOR * , RECTL*);
typedef VOID (FNXFER)(PDEV*, LONG, RECTL*, ULONG, SURFOBJ*, POINTL*,
                      RECTL*, XLATEOBJ*,ULONG,BLENDOBJ*);
typedef VOID (FNCOPY)(PDEV*, LONG, RECTL*, ULONG, POINTL*, RECTL*, BLENDOBJ*);
typedef VOID (FNCOLORKEYBLT)(PDEV*, LONG, RECTL*, POINTL*, RECTL*, ULONG);
typedef VOID (FNFASTPATREALIZE)(PDEV*, RBRUSH*, POINTL*, BOOL);
typedef VOID (FNIMAGETRANSFER)(PDEV*, BYTE*, LONG, LONG, LONG, ULONG);
typedef BOOL (FNTEXTOUT)(SURFOBJ*, STROBJ*, FONTOBJ*, CLIPOBJ*, RECTL*,
                         BRUSHOBJ*, BRUSHOBJ*);
typedef VOID (FNLINETOTRIVIAL)(PDEV*, LONG, LONG, LONG, LONG, ULONG, MIX);
typedef VOID (FNLINETOCLIPPED)(PDEV*, LONG, LONG, LONG, LONG, ULONG, MIX, RECTL*);
typedef VOID (FNSETPALETTE)(PDEV*, ULONG, ULONG);

typedef VOID (FNWAITENGINEBUSY)(PDEV*);
typedef VOID (FNWAITCHANNELSWITCH)(PDEV*);
typedef ULONG (FNDMAPUSHGO)(PDEV*, ULONG, ULONG, ULONG, ULONG, ULONG, ULONG, ULONG);
typedef BOOL (FNSTROKEPATH)(SURFOBJ *,PATHOBJ *,CLIPOBJ *,XFORMOBJ*,BRUSHOBJ *,POINTL *,LINEATTRS *, MIX);
typedef BOOL (FNLINETO)(SURFOBJ *,CLIPOBJ *,BRUSHOBJ *,LONG,LONG,LONG,LONG,RECTL *,MIX);
typedef BOOL (FNSTRETCHCOPY)(PDEV*, ULONG, int, RECTL*, RECTL*, int, RECTL*);

typedef VOID (FastXFER)(PDEV*, RECTL*, SURFOBJ*, POINTL*, XLATEOBJ*);

typedef VOID (FNACQUIREOGLMUTEX)(PDEV*);
typedef VOID (FNRELEASEOGLMUTEX)(PDEV*);

//******************************************************************************
// NV1 Function declarations
//******************************************************************************


FNWAITENGINEBUSY    NV1_WaitWhileGraphicsEngineBusy;
FNSTROKEPATH        NV1StrokePath;
FNLINETO            NV1LineTo;

//******************************************************************************
// NV3 Function declarations
// (These functions support 16bpp in 555 format)
//******************************************************************************

FNSETDESTBASE       NV3_SetDestBase;
FNSETSOURCEBASE     NV3_SetSourceBase;
FNFILL              NV3FillPatFast;
FNFILL              NV3FillSolid;
FNMEMTOSCRN         NV3MemToScreenBlt;
FNMEMTOSCRNPATTERN  NV3MemToScreenWithPatternBlt;
FNXFER              NV3Xfer1bpp;
FNXFER              NV3Xfer4bpp;
FNXFER              NV3XferNative;
FNCOPY              NV3CopyBlt;
FNTEXTOUT           NV3TextOut;
FNSETPALETTE        NV3SetPalette;

FNWAITENGINEBUSY    NV3_WaitWhileGraphicsEngineBusy;
FNWAITCHANNELSWITCH NV3_WaitForChannelSwitch;
FNDMAPUSHGO         NV3_DmaPushGo;

FNACQUIREOGLMUTEX   NV3_AcquireOglMutex;
FNRELEASEOGLMUTEX   NV3_ReleaseOglMutex;

FNSCRNTO1BPPMEM     NV4ScreenTo1bppMemBlt;

FNSCRNTOMEM         NV4ScreenToMemBlt;
FNSCRNTOMEM         NV4ScreenToMem16to4bppBlt;
FNSCRNTOMEM         NV4ScreenToMem32to4bppBlt;
FNSCRNTOMEM         NV4ScreenToMem16to8bppBlt;
FNSCRNTOMEM         NV4ScreenToMem32to8bppBlt;
FNSCRNTOSCRNPAT     NV4ScreenToScreenWithPatBlt;

FNWAITENGINEBUSY    NV4_WaitWhileGraphicsEngineBusy;
FNWAITCHANNELSWITCH NV4_WaitForChannelSwitch;

FNACQUIREOGLMUTEX   NV4_AcquireOglMutex;
FNRELEASEOGLMUTEX   NV4_ReleaseOglMutex;
VOID NV4_EnableDither(PDEV*  ppdev, BOOLEAN DitherFlag);

//******************************************************************************
// NV4 DMA Pusher Function declarations
// (These functions support 16bpp in 565 format)
//******************************************************************************

FNSETDESTBASE       NV4_DmaPushSetDestBase;
FNSETSOURCEBASE     NV4_DmaPushSetSourceBase;
FNFILL              NV4DmaPushFillPatFast;
FNFILL              NV4DmaPushFillSolid;
FNMEMTOSCRN         NV4DmaPushMemToScreenBlt;
FNMEMTOSCRNPATTERN  NV4DmaPushMemToScreenWithPatternBlt;
FNXFER              NV4DmaPushXfer1bpp;
FNXFER              NV4DmaPushXfer4bpp;
FNXFER              NV4DmaPushXferNative;
FNCOPY              NV4DmaPushCopyBlt;
FNCOLORKEYBLT       NV4DmaPushColorKeyBlt;
FNTEXTOUT           NV4DmaPushTextOut;
FNSETPALETTE        NV4DmaPushSetPalette;

FNXFER              NV4DmaPushXfer4to16bpp;
FNXFER              NV4DmaPushXfer8to16bpp;
FNXFER              NV4DmaPushXfer4to32bpp;
FNXFER              NV4DmaPushXfer8to32bpp;
FNXFER              NV4DmaPushIndexedImage;

FastXFER            NV4DmaPushFastXfer8to32;
FastXFER            NV4DmaPushFastXfer8to16;

FNGETSCRNBITS       NV4DmaPushDMAGetScreenBits;

FNWAITENGINEBUSY    NV4_DmaPushWaitWhileGraphicsEngineBusy;
FNWAITCHANNELSWITCH NV4_DmaPushWaitForChannelSwitch;
FNSTROKEPATH        NV4DmaPushStrokePath;
FNLINETO            NV4DmaPushLineTo;
FNSTRETCHCOPY       NV4DmaPushStretchCopy;

//******************************************************************************
// NV10 DMA Pusher Function declarations
// (These functions support 16bpp in 565 format)
//******************************************************************************

FNXFER              NV4DmaPushIndexedImage;     // this function call NV10DmaPush, but include
                                                // a bugfix for bug #20000720-213841
FNXFER              NV10DmaPushIndexedImage;
FNSETDESTBASE       NV10_DmaPushSetDestBase;
FNSETSOURCEBASE     NV10_DmaPushSetSourceBase;


FNFASTPATREALIZE    vMmFastPatRealize;
FNIMAGETRANSFER     vMmImageTransferMm16;
FNIMAGETRANSFER     vMmImageTransferMm32;
FNLINETOTRIVIAL     vMmLineToTrivial;
FNLINETOCLIPPED     vMmLineToClipped;


//////////////////////////////////////////////////////////////////////
// Function pointer for Direct Draw
//////////////////////////////////////////////////////////////////////

typedef ULONG (FNGRAPHICSENGINEBUSY)(PDEV*);
typedef VOID (FNFLIPBUFFER)(PDEV*,ULONG);
typedef ULONG (FNVBLANKISACTIVE)(PDEV*);
typedef VOID (FNWAITWHILEVBLANKACTIVE)(PDEV*);
typedef VOID (FNWAITWHILEDISPLAYACTIVE)(PDEV*);
typedef ULONG (FNGETSCANLINE)(PDEV*);
typedef VOID (FNDDENABLE)(PDEV*);
typedef DWORD (FNDDUPDATEFLIPSTATUS)(PDEV*, DWORD);

typedef ULONG (FNFIFOISBUSY)(PDEV *);

FNGRAPHICSENGINEBUSY    NV3_GraphicsEngineBusy;
FNGRAPHICSENGINEBUSY    NV4_GraphicsEngineBusy;
FNFLIPBUFFER            NV3_FlipBuffer;
FNVBLANKISACTIVE        NV3_VBlankIsActive;
FNVBLANKISACTIVE        NV4_VBlankIsActive;
FNWAITWHILEVBLANKACTIVE NV3_WaitWhileVBlankActive;
FNWAITWHILEVBLANKACTIVE NV4_WaitWhileVBlankActive;
FNWAITWHILEDISPLAYACTIVE NV3_WaitWhileDisplayActive;
FNWAITWHILEDISPLAYACTIVE NV4_WaitWhileDisplayActive;
FNGETSCANLINE           NV_GetScanLineData;

FNFIFOISBUSY            NV1FifoIsBusy;

VOID vPutBits(PDEV*, DSURF *,SURFOBJ*, RECTL*);
VOID vGetBits(PDEV*, DSURF *,SURFOBJ*, RECTL*);

VOID vIoSlowPatRealize(PDEV*, RBRUSH*, BOOL);

VOID NvHwSpecific(PDEV* ppdev);
//******************************************************************************
// Capabilities flags
//
// These are private flags passed to us from the NV miniport.  They
// come from the 'DriverSpecificAttributeFlags' field of the
// 'VIDEO_MODE_INFORMATION' structure (found in 'ntddvdeo.h') passed
// to us via an 'VIDEO_QUERY_AVAIL_MODES' or 'VIDEO_QUERY_CURRENT_MODE'
// IOCTL.
//
// NOTE: These definitions must match those in the NV miniport's 'NV.h'!
//******************************************************************************

//******************************************************************************
// Do NOT use CAPS_NVX_IS_PRESENT anywhere in the display driver.  Chip
// capabilities should be based on the classes that are allocated.  See
// ppdev->CurrentClass for which classes got allocated at init time.
//******************************************************************************


typedef enum {
/*    
    CAPS_NV1_IS_PRESENT     = 0x00000010,   // NV1 Chip is present
    CAPS_NV3_IS_PRESENT     = 0x00000020,   // NV3 Chip is present
    CAPS_NV4_IS_PRESENT     = 0x00000040,   // NV4 Chip is present
    CAPS_NV5_IS_PRESENT     = 0x00000080,   // NV5 Chip is present
    CAPS_NV5VANTA_IS_PRESENT= 0x00000100,   // NV5 VANTA Chip is present
    CAPS_NV5ULTRA_IS_PRESENT= 0x00000200,   // NV5 ULTRA Chip is present
    CAPS_NV0A_IS_PRESENT    = 0x00000400,   // NV0A Chip is present
    CAPS_NV10_IS_PRESENT    = 0x00000800,   // NV10 Chip is present
 CAPS_NV5MODEL64_IS_PRESENT = 0x00001000,   // NV5 Model 64 is present
 CAPS_NV10DDR_IS_PRESENT    = 0x00002000,   // NV10 DDR Chip is present
 CAPS_NV10GL_IS_PRESENT     = 0x00004000,   // NV10 GL Chip is present
 CAPS_NV11_IS_PRESENT       = 0x00008000,   // NV11 Chip is present
 CAPS_NV11DDR_IS_PRESENT    = 0x00010000,   // NV11 DDR Chip is present
 CAPS_NV11M_IS_PRESENT      = 0x80000000,   // NV11M Chip is present
 CAPS_NV11GL_IS_PRESENT     = 0x00020000,   // NV11 GL Chip is present
 CAPS_NV15_IS_PRESENT       = 0x00040000,   // NV15 Chip is present
 CAPS_NV15DDR_IS_PRESENT    = 0x00080000,   // NV15 DDR Chip is present
 CAPS_NV15GL_IS_PRESENT     = 0x00100000,   // NV15 GL Chip is present
 CAPS_NV15BR_IS_PRESENT     = 0x00200000,   // NV15 BR Chip is present
 CAPS_NV20_IS_PRESENT       = 0x00400000,   // NV20 Chip is present
*/    
// not used    CAPS_BT485_POINTER      = 0x00400000,   // Use Brooktree 485 pointer
// not used     CAPS_TI025_POINTER      = 0x00800000,   // Use TI TVP3020/3025 pointer
// not used     CAPS_SCALE_POINTER      = 0x01000000,   // Set if the S3 hardware pointer
// not used                                             //   x position has to be scaled by
// not used                                             //   two
// not used     CAPS_SPARSE_SPACE       = 0x02000000,   // Frame buffer is mapped in sparse
// not used                                             //   space on the Alpha
// not used     CAPS_NEW_BANK_CONTROL   = 0x04000000,   // Set if 801/805/928 style banking
// not used     CAPS_NEWER_BANK_CONTROL = 0x08000000,   // Set if 864/964 style banking
// not used     CAPS_RE_REALIZE_PATTERN = 0x10000000,   // Set if we have to work around the
// not used                                             //   864/964 hardware pattern bug
// not used     CAPS_SLOW_MONO_EXPANDS  = 0x20000000,   // Set if we have to slow down
// not used                                             //   monochrome expansions
    CAPS_SW_POINTER         = 0x40000000,   // No hardware pointer; use software simulation
// Note definition above: CAPS_NV11M_IS_PRESENT      = 0x80000000,   // NV11M Chip is present


} CAPS;

#define CAPS_DAC_POINTER        (CAPS_BT485_POINTER | CAPS_TI025_POINTER)

#define CAPS_LINEAR_FRAMEBUFFER CAPS_NEW_MMIO
                                            // For now, we're linear only
                                            //   when using with 'New MM I/O'

//******************************************************************************
// Do NOT use NVX_IS_PRESENT anywhere in the display driver.  Chip
// capabilities should be based on the classes that are allocated.  See
// ppdev->CurrentClass for which classes got allocated at init time.
//******************************************************************************

/*
// The NV4_IS_PRESENT bit is also used as a compatibility flag, so to truly detect an NV4, no other bits must be set
#define IS_NV04_PRESENT(ppdev) ((ppdev->flCaps & CAPS_NV4_IS_PRESENT) && (ppdev->flCaps & ~CAPS_NV4_IS_PRESENT)==0)

#define IS_NV05_PRESENT(ppdev)  ((ppdev->flCaps & CAPS_NV5_IS_PRESENT) ||\
             (ppdev->flCaps & CAPS_NV5VANTA_IS_PRESENT) ||\
             (ppdev->flCaps & CAPS_NV5MODEL64_IS_PRESENT) ||\
             (ppdev->flCaps & CAPS_NV5ULTRA_IS_PRESENT) ||\
             (ppdev->flCaps & CAPS_NV0A_IS_PRESENT))

#define IS_NV10_PRESENT(ppdev)  ((ppdev->flCaps & CAPS_NV10_IS_PRESENT) ||\
             (ppdev->flCaps & CAPS_NV10DDR_IS_PRESENT) ||\
             (ppdev->flCaps & CAPS_NV10GL_IS_PRESENT))

#define IS_NV11_PRESENT(ppdev)  ((ppdev->flCaps & CAPS_NV11_IS_PRESENT) ||\
             (ppdev->flCaps & CAPS_NV11DDR_IS_PRESENT) ||\
             (ppdev->flCaps & CAPS_NV11M_IS_PRESENT)   ||\
             (ppdev->flCaps & CAPS_NV11GL_IS_PRESENT))

#define IS_NV15_PRESENT(ppdev)  ((ppdev->flCaps & CAPS_NV15_IS_PRESENT) ||\
             (ppdev->flCaps & CAPS_NV15DDR_IS_PRESENT) ||\
             (ppdev->flCaps & CAPS_NV15BR_IS_PRESENT)  ||\
             (ppdev->flCaps & CAPS_NV15GL_IS_PRESENT))

#define IS_NV20_PRESENT(ppdev)  ((ppdev->flCaps & CAPS_NV20_IS_PRESENT))

*/

// DENSE(ppdev) returns TRUE if the normal 'dense space' mapping of the
// frame buffer is being used.  It returns FALSE only on the Alpha when
// the frame buffer is mapped in using 'sparse space,' meaning that all
// reads and writes to and from the frame buffer must be done through the
// funky 'ioaccess.h' macros.

#if defined(_ALPHA_)
    #define DENSE(ppdev)        (!(ppdev->flCaps & CAPS_SPARSE_SPACE))
#else
    #define DENSE(ppdev)        1
#endif

////////////////////////////////////////////////////////////////////////
// Status flags

typedef enum {
    STAT_GLYPH_CACHE        = 0x0001,   // Glyph cache successfully allocated
    STAT_BRUSH_CACHE        = 0x0002,   // Brush cache successfully allocated
    STAT_DIRECTDRAW_CAPABLE = 0x0004,   // Card is DirectDraw capable
    STAT_DIRECTDRAW_ENABLED = 0x0008,   // DirectDraw is currently enabled
    STAT_STREAMS_ENABLED    = 0x0010,   // Streams are enabled for this mode
    STAT_FORMATTER_ENABLED  = 0x0020,   // Pixel formatter enabled for this mode
} STATUS;

////////////////////////////////////////////////////////////////////////
// Nvidia OpenGL client information.
// globalOpenGLData.oglClientCount will not go to 0, so it is bad to use it for ogl tests!
#define OglIsEnabled(_pdev) (0 != globalOpenGLData.oglDrawableClientCount)
#define OGL_FLIPPED() (ppdev->ulPrimarySurfaceOffset != ppdev->ulFlipBase)

#define NV_OGL_CLIENT_DRAWABLE_INFO 0x0001 // if bit ON then drawable info exists...
#define NV_OGL_CLIENT_CONTEXT_INFO  0x0002 // if bit ON then context info exits...
#define NV_OGL_CLIENT_SHARED_INFO   0x0004 // if bit ON then context info exits...
#define NV_OGL_CLIENT_WOC_DELETE    0x0008 // if bit ON then WOC_DELETE occured on window...

typedef struct _NV_OPENGL_CLIENT_INFO
    {
    DWORD                      flags;               // bits indicating allocated/free resources
    HDRVOBJ                    hDrvObj;             // driver object handle for this client
    WNDOBJ                     *pwo;                // WNDOBJ pointer for drawable node
    struct _PDEV               *ppdev;              // Pointer to current ppdev;
    DWORD                      modeSwitchCount;     // mode switch counter when hDrvObj is created
    ULONG                      processHandle;       // Process handle from client associated with globalData
    PVOID                      oglGlobalPagePtr;    // Pointer to shared memory
    PVOID                      pfifoAddress;        // Address of FIFO in client address space
    PVOID                      pbusAddress;         // Address of PBUS in client address space
    ULONG                      hClient;             // Per-process shared hClient
    PIXELFORMATDESCRIPTOR      pfd;                 // pixelformat of the client window
    BOOL                       bStereoDisabled;     // if stereo pfd: TRUE: currently disabled because unused
    BOOL                       bFlipping;           // TRUE if client is allowed to flip.
    DWORD                      dwProcessID;         // ProcessID of ICD's process
    struct _NV_OPENGL_DRAWABLE_INFO    *clientDrawableInfo; // drawable information for this client
    struct _NV_OPENGL_CONTEXT_INFO     *clientContextInfo;  // context information for this client
    } NV_OPENGL_CLIENT_INFO;

struct _NV_OPENGL_CLIENT_INFO_LIST;
typedef struct _NV_OPENGL_CLIENT_INFO_LIST
    {
        ULONG refCount;
        NV_OPENGL_CLIENT_INFO              clientInfo;
        struct _NV_OPENGL_CLIENT_INFO_LIST *next;
    } NV_OPENGL_CLIENT_INFO_LIST;

typedef struct _NV_OPENGL_CLIENT_LIST_HEAD
    {
    NV_OPENGL_CLIENT_INFO_LIST *clientList;
    } NV_OPENGL_CLIENT_LIST_HEAD;

typedef struct _NV_LAYER_PALETTE
    {
    struct _NV_LAYER_PALETTE *next;

    void         *windowHandle;
    unsigned int  entries[ICD_PALETTE_ENTRIES];
    unsigned int  crTransparent;
    } NV_LAYER_PALETTE;

typedef struct _NV_LAYER_PALETTE_LIST_HEAD
    {
    NV_LAYER_PALETTE *paletteList;
    } NV_LAYER_PALETTE_LIST_HEAD;

typedef struct _GLOBAL_OPENGL_DATA {
    NV_OPENGL_CLIENT_LIST_HEAD oglClientListHead;
    ULONG                      oglClientCount; // Count of number of OGL clients connected to display driver
    ULONG                      oglDrawableClientCount; // Count of number of OGL drawable clients
    // RBierman: addons for overlay
    NV_LAYER_PALETTE_LIST_HEAD oglLayerPaletteListHead;
    ULONG                      oglLayerPaletteClientCount; 
#ifdef NV_MAIN_MULTIMON
    struct _LINKEDLISTHEAD     *oglpdevList;
#endif
} GLOBAL_OPENGL_DATA;

extern GLOBAL_OPENGL_DATA globalOpenGLData;
extern VOID OglAcquireGlobalMutex();
extern VOID OglReleaseGlobalMutex();

typedef struct _CLASSOBJ {
    ULONG ChannelDMA;
    ULONG IndexedImageFromCPU;
    ULONG VideoLutCursorDac;
    ULONG ContextSurfaces2D;
    ULONG VideoOverlay;
    ULONG ScaledImageFromMemory;
    ULONG AlphaImageFromMemory;
} CLASSOBJ;

////////////////////////////////////////////////////////////////////////
// Misc video related defs
#define NV_MAX_BUFFERS 8

typedef struct _VIDBUFFER
{
     ULONG offset;                 /* byte offset of top-left pixel       00-  03*/
     ULONG pitch;                  /* bytes between lines                 04-  07*/
     ULONG size;                   /* height_width U16_U16 in pixels      08-  0b*/
     ULONG format;                 /* ORed NV_VFM_IMAGE_FORMAT_*          0c-  0f*/
     ULONG notify;                 /* notification style, varies w/OS     10-  13*/
} VIDBUFFER, *PVIDBUFFER;

// for ICD-Overlay purposes + DDrawApp detection
#define MAX_POINTER_LIST_ELEMENTS 50 

typedef struct _POINTER_LIST
{
    ULONG  cElements;
    PULONG apulElements[MAX_POINTER_LIST_ELEMENTS];
}POINTER_LIST, *PPOINTER_LIST;

////////////////////////////////////////////////////////////////////////
// HW workaround flags
#define NV_WORKAROUND_NV11RevB_DIRTHERING       0x00000001

// this is required for pre-win2k driver builds. on win2k, the display
// driver shares a flip record in GLOBALDATA with the d3d driver
typedef struct 
{
    FLATPTR         fpFlipFrom;             // Surface we last flipped from
    FLATPTR         fpFlipTo;               // Surface we last flipped to
    LONGLONG        liFlipTime;             // Time at which last flip
                                            //   occured
    LONGLONG        liFlipDuration;         // Precise amount of time it
                                            //   takes from vblank to vblank
    BOOL            bHaveEverCrossedVBlank; // True if we noticed that we
                                            //   switched from inactive to
                                            //   vblank
    BOOL            bWasEverInDisplay;      // True if we ever noticed that
                                            //   we were inactive
    UINT            dwFlipScanLine;
    BOOL            bFlipFlag;              // True if we think a flip is
                                            //   still pending
    DWORD           fpPreviousFlipFrom;     // Previous surface we flipped from
    LONGLONG        liPreviousFlipTime;     // Time of previous surface flip
    DWORD           dwReserved1;            // pad to 64 bits
} OLD_FLIPRECORD;

//
// cliplist structure managed through cliplist.c/h
typedef struct _CLIPLIST
{
  RECTL   rclBounds;  // visible bounding <= rclClient!
  LONG    cMax;       // allocated size for prcl
  LONG    c;          // number of rects stored in prcl
  LONG    cPixel;     // Number of total pixels inside cliplist
  RECTL  *prcl;       // array of size cMac with c valid rects
} CLIPLIST;


////////////////////////////////////////////////////////////////////////
// The Physical Device data structure

typedef struct  _PDEV
{
    // -------------------------------------------------------------------
    // NOTE: Changes between here and NOTE1 in the PDEV structure must be
    // reflected in i386\strucs.inc (assuming you're on an x86, of course)!

    ULONG       NVSignature;
    LONG        xOffset;                // Pixel offset from (0, 0) to current
    LONG        yOffset;                //   DFB located in off-screen memory
    DWORD*      pjMmBase;               // We'll use it as a ptr to our NV Channel
    volatile DWORD* GrStatusReg;        // We'll use it as a ptr to our Gr Status Reg
    volatile DWORD* FbConfig0Reg;       // We'll use it as a ptr to our Fb Config0 Reg
    volatile DWORD* FbStartAddr;        // We'll use it as a ptr to our Fb Start Addr
    volatile DWORD* DACRegs;            // We'll use it as a ptr to our Cursor registers
    volatile UCHAR* PRMCIORegs;         // NV3 PRMCIO Regs
    volatile UCHAR* PRMVIORegs;         // NV3 PRMVIO Regs
    volatile DWORD* PRAMDACRegs;        // NV3 PRAMDAC Regs
    volatile DWORD* PRAMINRegs;         // NV3 PRAMIN Regs
    volatile DWORD* PGRAPHRegs;         // NV3 PGRAPH Regs
    volatile DWORD* PFIFORegs;          // NV3 PFIFO Regs
    ULONG       FbBufferToggle;         // 0 or 1 (denotes frame buffer)
    BYTE*       pjFrameBufbase;         // pjFrameBuf base pointing to the base of frame buffer
    BYTE*       pjScreen;               // Points to base screen address
    LONG        lDelta;                 // Distance from one scan to the next.
    LONG        cjPelSize;              // 1 if 8bpp, 2 if 16bpp, 3 if 24bpp,
                                        //   4 if 32bpp
    ULONG       iBitmapFormat;          // BMF_8BPP or BMF_16BPP or BMF_32BPP
                                        //   (our current colour depth)
    LONG        iBoard;                 // Logical multi-board identifier
                                        //   (zero by default)
    ULONG       fontCacheOffset;        // Offset into font cache offscreen mem area
    ULONG       fontCacheEnabled;       // Flag to indicate whether driver will cache fonts

    CAPS        flCaps;                 // Capabilities flags
    STATUS      flStatus;               // Status flags
    BOOL        bEnabled;               // In graphics mode (not full-screen)

    HANDLE      hDriver;                // Handle to \Device\Screen
    HDEV        hdevEng;                // Engine's handle to PDEV

    LONG        cxScreen;               // Visible screen width
    LONG        cyScreen;               // Visible screen height
    LONG        cxMemory;               // Width of Video RAM
    LONG        cyMemory;               // Height of Video RAM
    LONG        cBitsPerPel;            // Bits per pel (8, 15, 16, 24 or 32)
    ULONG       cbFrameBuf;             // Size of frame buffer in bytes
    ULONG       AlphaEnableValue;       // Global alpha enable value

    ////////// DMA Pusher Functionality

    ULONG       DmaPushEnabled2D;
    ULONG       DmaPushBufTotalSize;
    ULONG       DmaPushBufCanUseSize;
    ULONG       DmaPushBufMinSize;
    ULONG       DmaPushHandle;
    ULONG      *nvDmaFifo;
    ULONG       nvDmaCount;
    ULONG      *nvDmaChannel;
    ULONG       nvDmaCachedGet;
    ULONG       nvDmaCachedPut;
    ULONG       nvDmaPreviousPut;
    ULONG       nvDmaWrapFlag;
    LONG        nvDmaCachedFreeCount;
    ULONG      *nvDmaFlushFixDwordPtr;
    ULONG       nvDmaFlushFixDummyValue;

    // -------------------------------------------------------------------
    // NOTE1: Changes up to here in the PDEV structure must be reflected in
    // i386\strucs.inc (assuming you're on an x86, of course)!

    ULONG       left;                   // left edge of display (relative to desktop)
    ULONG       top;                    // rop edge of display (relative to desktop)

    volatile DWORD* NvBaseAddr;         // Base address of NV device regs
    VOID*  NvDmaIndexedImageLut;        // pointer to indexed image LUT buffer
    ULONG  NvLastIndexedImageNotifier;

    ULONG       AliFixupIoNeeded;       // Flag to enable IO fixup for ALI
    ULONG       AliFixupIoBase;         // Mapping to unused IO register

    ULONG       ulMode;                 // Mode the mini-port driver is in.

    FLONG       flHooks;                // What we're hooking from GDI
    FLONG       flHooksDeviceBitmap;    // What we're hooking from GDI
    ULONG       ulWhite;                // 0xff if 8bpp, 0xffff if 16bpp,
                                        //   0xffffffff if 32bpp
    VOID*       pMemToMemBuffer;        // General purpose temporary buffer,
                                        //   MEM_TO_MEM_BUFFER_SIZE bytes in size
                                        //   (Remember to synchronize if you
                                        //   use this for device bitmaps or
                                        //   async pointers)
    ULONG       MemToMemBufferSize;     // Holds size of mem to mem temporary buffer
    USHORT*     apwMmXfer[XFER_BUFFERS];// Pre-computed array of unique
    ULONG*      apdMmXfer[XFER_BUFFERS];//   addresses for doing memory-mapped
                                        //   transfers without memory barriers
                                        // Note that the 868/968 chips have a
                                        //   hardware bug and can't do byte
                                        //   transfers
    HSEMAPHORE  csCrtc;                 // Used for synchronizing access to
                                        //   the CRTC register
    HSEMAPHORE  csFifo;                 // Used for synchronizing access to
                                        //   the DMA pusher or CPU pusher shared FIFO
    ULONG       numDevices;             // Number of devices OpenGL is using...
    ULONG       bSupportOgl;            // TRUE if OpenGL ICD is supported
    ULONG       bReleaseOglMutex;       // TRUE indicates driver must release the OGL mutex
    ULONG       *oglGlobalPagePtr;      // pointer to page of locked down memory used by OGL
    PVOID       origOglGlobalPagePtr;   // Pointer remembered to free oglGlobalPagePtr
    PVOID       oglGlobalPageMdl;       // MDL to locked down OGL page
    ULONG       oglGlobalPageSize;      // size in bytes of locked down OGL page
    ULONG       *oglMutexPtr;           // OGL mutex lock pointer
    ULONG       *oglModeSwitchPtr;      // OGL mode switch pointer
    ULONG       bOglSingleBackDepthSupported; // TRUE if display driver allocates single-back-depth surface
    ULONG       bOglSingleBackDepthCreated; // TRUE if display driver allocated single-back-depth surface
    ULONG       oglSyncGdi;             // Synchronize OpenGL with GDI
    ULONG       ulOpenGLinFullScreen;   // TRUE if OpenGL is rendering in FULL screen
    ULONG       hwGfxCaps;              // Graphics caps reported by the RM. 
    DWORD       oglRegUBB;              // registry val for NV_REG_OGL_SINGLE_BACKDEPTH_BUFFER
    DWORD       oglRegFlippingControl;  // registry val for NV4_REG_OGL_BUFFER_FLIPPING_CONTROL
    DWORD       oglRegWindowFlipping;   // registry val for NV_REG_OGL_WINDOW_FLIPPING
    DWORD       oglRegOverlaySupport;   // registry val for NV_REG_OGL_OVERLAY_SUPPORT
    DWORD       oglRegApiStereoSupport; // registry val for NV_REG_OGL_STEREO_SUPPORT
    DWORD       oglRegApiStereoMode;    // registry val for NV_REG_OGL_API_STEREO_MODE
    BOOL        bOglOverlayDesired;     // reflects that ogl overlay is switched on via registry
    BOOL        bOglStereoDesired;      // reflects that ogl stereo is switched on via registry
    BOOL        bOglStereoActive;       // TRUE if stereo is active now, and glasses are in any of oglRegApiStereoMode supported modes
    ULONG       ulOglActiveViews;       // number of addition active OGL_STEREO_NUMBUFFERS views for ogl 0 -> ogl not running, 1 -> monoscopic left only, 2 -> stereo left and right
    NvU32       singleFrontOffset[OGL_STEREO_NUMBUFFERS]; // offset in video memory to single-front buffer
    NvU32       singleFrontPitch[OGL_STEREO_NUMBUFFERS];  // offset in video memory to single-front buffer
    NvU32       singleBack1Offset[OGL_STEREO_NUMBUFFERS]; // offset in video memory to single-back buffer
    NvU32       singleBack1Pitch[OGL_STEREO_NUMBUFFERS];  // offset in video memory to single-back buffer
    NvU32       singleDepthOffset[OGL_STEREO_NUMBUFFERS]; // offset in video memory to single-depth buffer
    NvU32       singleDepthPitch[OGL_STEREO_NUMBUFFERS];  // offset in video memory to single-depth buffer
    ULONG       ulTempOffscreenBufferSize;  // size of temporary offscreen buffer ( lDelta long )
    ULONG       ulTempOffscreenBufferOffset;// offset of temporary offscreen buffer ( needed in ubb mode )
    ULONG       bOglOverlaySurfacesCreated; // TRUE if display driver allocated overlay surfaces
                                            //      for OpenGL
    NvU32       singleFrontMainOffset[OGL_STEREO_NUMBUFFERS];      // Offsets and pitches to main and overlay planes
    NvU32       singleFrontMainPitch[OGL_STEREO_NUMBUFFERS];       //    for opengl pseudo hardware implementation of
    NvU32       singleBack1MainOffset[OGL_STEREO_NUMBUFFERS];      //    overlay planes
    NvU32       singleBack1MainPitch[OGL_STEREO_NUMBUFFERS]; 
    NvU32       singleFrontOverlayOffset[OGL_STEREO_NUMBUFFERS];
    NvU32       singleFrontOverlayPitch[OGL_STEREO_NUMBUFFERS]; 
    NvU32       singleBack1OverlayOffset[OGL_STEREO_NUMBUFFERS];
    NvU32       singleBack1OverlayPitch[OGL_STEREO_NUMBUFFERS]; 
    POINTER_LIST plSavedPopupMenus; // MSchwarzer 10/04/2000 need this for overlay popuprestauration
    LONG        lSavedScreenBits;         // MSchwarzer 11/20/2000 need this for overlay popuprestauration
    FNACQUIREOGLMUTEX *pfnAcquireOglMutex; // function to acquire the OGL mutex
    FNRELEASEOGLMUTEX *pfnReleaseOglMutex; // function to release the OGL mutex
    CLIPLIST          OglNegativeClipList; // pointer to negative cliplist == desktop - all clients

    ////////// Low-level blt function pointers:

    FNSETDESTBASE*      pfnSetDestBase;
    FNSETSOURCEBASE*    pfnSetSourceBase;

    FNFILL*             pfnFillSolid;
    FNFILL*             pfnFillPat;
    FNXFER*             pfnXfer1bpp;
    FNXFER*             pfnXfer4bpp;
    FNXFER*             pfnXferNative;
    FNXFER*             pfnXfer8to16bpp;
    FNXFER*             pfnXfer4to16bpp;
    FNXFER*             pfnXfer8to32bpp;
    FNXFER*             pfnXfer4to32bpp;
    FastXFER*           pfnFastXfer8to32;
    FastXFER*           pfnFastXfer8to16;

    FNXFER*             pfnIndexedImage;
    FNCOPY*             pfnCopyBlt;
    FNMEMTOSCRN*        pfnMemToScreenBlt;
    FNSCRNTO1BPPMEM*    pfnScreenTo1BppMemBlt;
    FNGETSCRNBITS*      pfnGetScreenBits;
    FNFASTPATREALIZE*   pfnFastPatRealize;
    FNIMAGETRANSFER*    pfnImageTransfer;
    FNTEXTOUT*          pfnTextOut;
    FNSETPALETTE*       pfnSetPalette;
    FNLINETOTRIVIAL*    pfnLineToTrivial;
    FNLINETOCLIPPED*    pfnLineToClipped;

    
    FNSCRNTOMEM*        pfnScreenToMemBlt;        // 
                                                  //
    DWORD               globalXferOverheadFactor; // This is not a pfn, but is data
                                                  // needed by pfnScreenToMemBlt

    FNSCRNTOMEM*        pfnScreenToMem16to4bppBlt; 
    FNSCRNTOMEM*        pfnScreenToMem32to4bppBlt; 
    FNSCRNTOMEM*        pfnScreenToMem16to8bppBlt; 
    FNSCRNTOMEM*        pfnScreenToMem32to8bppBlt; 

    FNSCRNTOSCRNPAT*    pfnScreenToScreenWithPatBlt; 


    ////////// Other NV function pointers:

    FNWAITENGINEBUSY*   pfnWaitEngineBusy;
    FNWAITCHANNELSWITCH* pfnWaitForChannelSwitch;
    FNDMAPUSHGO*        pfnDmaPushGo;
    FNMEMTOSCRNPATTERN* pfnMemToScreenPatternBlt;
    FNSTROKEPATH*       pfnStrokePath;
    FNLINETO*           pfnLineTo;
    FNSTRETCHCOPY*      pfnStretchCopy;

    ////////// Eng fallback function pointers:
    
    BOOL (*pfnEngBitBlt)(SURFOBJ  *psoTrg,
                                  SURFOBJ  *psoSrc,
                                  SURFOBJ  *psoMask,
                                  CLIPOBJ  *pco,
                                  XLATEOBJ *pxlo,
                                  RECTL    *prclTrg,
                                  POINTL   *pptlSrc,
                                  POINTL   *pptlMask,
                                  BRUSHOBJ *pbo,
                                  POINTL   *pptlBrush,
                                  ROP4      rop4);
    BOOL (*pfnEngCopyBits)(SURFOBJ  *psoDest,
                                    SURFOBJ  *psoSrc,
                                    CLIPOBJ  *pco,
                                    XLATEOBJ *pxlo,
                                    RECTL    *prclDest,
                                    POINTL   *pptlSrc);
    BOOL (*pfnEngAlphaBlend)(SURFOBJ       *psoDest,
                                      SURFOBJ       *psoSrc,
                                      CLIPOBJ       *pco,
                                      XLATEOBJ      *pxlo,
                                      RECTL         *prclDest,
                                      RECTL         *prclSrc,
                                      BLENDOBJ      *pBlendObj);
    BOOL (*pfnEngTransparentBlt)(SURFOBJ       *psoDest,
                                      SURFOBJ       *psoSrc,
                                      CLIPOBJ       *pco,
                                      XLATEOBJ      *pxlo,
                                      RECTL         *prclDest,
                                      RECTL         *prclSrc,
                                      ULONG         iTransColor,
                                      ULONG         Reserved);
    BOOL (*pfnEngTextOut)(SURFOBJ  *pso,
                                   STROBJ   *pstro,
                                   FONTOBJ  *pfo,
                                   CLIPOBJ  *pco,
                                   RECTL    *prclExtra,
                                   RECTL    *prclOpaque,
                                   BRUSHOBJ *pboFore,
                                   BRUSHOBJ *pboOpaque,
                                   POINTL   *pptlOrg,
                                   MIX       mix);
    BOOL (*pfnEngLineTo)(SURFOBJ   *pso,
                                  CLIPOBJ   *pco,
                                  BRUSHOBJ  *pbo,
                                  LONG       x1,
                                  LONG       y1,
                                  LONG       x2,
                                  LONG       y2,
                                  RECTL     *prclBounds,
                                  MIX        mix);
    BOOL (*pfnEngStrokePath)(SURFOBJ   *pso,
                                      PATHOBJ   *ppo,
                                      CLIPOBJ   *pco,
                                      XFORMOBJ  *pxo,
                                      BRUSHOBJ  *pbo,
                                      POINTL    *pptlBrushOrg,
                                      LINEATTRS *plineattrs,
                                      MIX        mix);

    struct {
        LONG  NumBuffers; // number of valid offsets in MultiBufferOffsets (signed to allow better error checking)
        ULONG MultiBufferOffsets[NV_MAX_BUFFERS];
        BOOL  bEnabled;

        struct {
            // FNACQUIREOGLMUTEX *pfnAcquireOglMutex; // function to acquire the OGL mutex
            // FNRELEASEOGLMUTEX *pfnReleaseOglMutex; // function to release the OGL mutex

            ////////// Low-level blt function pointers:
            // FNSETDESTBASE*      pfnSetDestBase;
            // FNSETSOURCEBASE*    pfnSetSourceBase;

            FNFILL*             pfnFillSolid;
            FNFILL*             pfnFillPat;
            FNXFER*             pfnXfer1bpp;
            FNXFER*             pfnXfer4bpp;
            FNXFER*             pfnXferNative;
            FNXFER*             pfnXfer8to16bpp;
            FNXFER*             pfnXfer4to16bpp;
            FNXFER*             pfnXfer8to32bpp;
            FNXFER*             pfnXfer4to32bpp;
            FastXFER*           pfnFastXfer8to32;
            FastXFER*           pfnFastXfer8to16;

            FNXFER*             pfnIndexedImage;
            FNCOPY*             pfnCopyBlt;
            FNMEMTOSCRN*        pfnMemToScreenBlt;
            FNSCRNTO1BPPMEM*    pfnScreenTo1BppMemBlt;
            //FNGETSCRNBITS*      pfnGetScreenBits;
            FNFASTPATREALIZE*   pfnFastPatRealize;
            FNIMAGETRANSFER*    pfnImageTransfer;
            FNTEXTOUT*          pfnTextOut;
            FNSETPALETTE*       pfnSetPalette;
            FNLINETOTRIVIAL*    pfnLineToTrivial;
            FNLINETOCLIPPED*    pfnLineToClipped;

            // Screen to Mem Function Pointers
            FNSCRNTOMEM*        pfnScreenToMemBlt;

            //DWORD               globalXferOverheadFactor; // This is not a pfn, but is data
            //                                              // needed by pfnScreenToMemBlt
            
            FNSCRNTOMEM*        pfnScreenToMem16to4bppBlt; 
            FNSCRNTOMEM*        pfnScreenToMem32to4bppBlt; 
            FNSCRNTOMEM*        pfnScreenToMem16to8bppBlt; 
            FNSCRNTOMEM*        pfnScreenToMem32to8bppBlt; 

            FNSCRNTOSCRNPAT*    pfnScreenToScreenWithPatBlt; 


            ////////// Other NV function pointers:

            // FNWAITENGINEBUSY*   pfnWaitEngineBusy;            
            // FNWAITCHANNELSWITCH* pfnWaitForChannelSwitch;
            // FNDMAPUSHGO*        pfnDmaPushGo;
            FNMEMTOSCRNPATTERN* pfnMemToScreenPatternBlt;
            FNSTROKEPATH*       pfnStrokePath;
            FNLINETO*           pfnLineTo;
            FNSTRETCHCOPY*      pfnStretchCopy;

        } SavedProcs;
    } DoubleBuffer;


    ////////// Palette stuff:

    PALETTEENTRY* pPal;                 // The palette if palette managed
    HPALETTE    hpalDefault;            // GDI handle to the default palette.
    FLONG       flRed;                  // Red mask for 16/32bpp bitfields
    FLONG       flGreen;                // Green mask for 16/32bpp bitfields
    FLONG       flBlue;                 // Blue mask for 16/32bpp bitfields
    FLONG       physicalColorMask;      //
    ULONG       cPaletteShift;          // number of bits the 8-8-8 palette must
                                        // be shifted by to fit in the hardware
                                        // palette.

    ////////// Primary surface stuff:
    
    HSURF       hsurfScreen;            // Engine's handle to screen surface
    DSURF       dsurfScreen;            // Our private DSURF for the screen
    OH          ohScreen;               // Off-screen heap structure for the
                                        //   visible screen

    ////////// Heap stuff:

    HEAP        heap;                   // All our off-screen heap data
    ULONG       iHeapUniq;              // Incremented every time room is freed
                                        //   in the off-screen heap

    DSURF*      pdsurf;                 // head of 2-link list of all DFBs.

    SURFOBJ*    psoPunt;                // Wrapper surface for having GDI draw
                                        //   on off-screen bitmaps
    SURFOBJ*    psoPunt2;               // Another one for off-screen to off-
                                        //   screen blts
    SURFOBJ*    psoPunt3;               // Another one needed for brush conversion

    ////////// Pointer stuff:

    ULONG       MonitorType;            // Cursor adjustment values for when
    ULONG       Underscan_x;            // the driver is running through a TV set
    ULONG       Underscan_y;            //
    ULONG       Scale_x;                //
    ULONG       Scale_y;                //
    ULONG       FilterEnable;           //
    ULONG       TVCursorMin;            //
    ULONG       TVCursorMax;            //

    LONG        xPointerHot;            // xHot of current hardware pointer
    LONG        yPointerHot;            // yHot of current hardware pointer

    LONG        xPointer;               // Start x-position for the current
                                        //   S3 pointer
    LONG        yPointer;               // Start y-position for the current

    ULONG       ulCursorMemOffset[2];   // Offset into inst mem of area for ptr bmps - Double buffer.
    ULONG       ulCursorCurBufIndex;    // Double Buffer, current cursor buffer;

    ULONG       ulCachedCursorWidth;    // Last width programmed into the HW
    ULONG       ulCachedCursorFormat;   // Last format programmed into the HW
    ULONG       ulMaxHwCursorDepth;     // Max color depth of cursor (16/32)
    ULONG       ulMaxHwCursorSize;      // Max cursor width/height supported by
                                        //   current hardware
    BOOL        bSupportAlphaCursor;    // Support Alpha blended cursor

    FNSHOWPOINTER*      pfnShowPointer;
    FNMOVEPOINTER*      pfnMovePointer;
    FNSETPOINTERSHAPE*  pfnSetPointerShape;
    FNENABLEPOINTER*    pfnEnablePointer;

    ////////// Brush stuff:
    LONG        iBrushCache;            // Index for next brush to be allocated
    LONG        cBrushCache;            // Total number of brushes cached
    BRUSHENTRY  abe[TOTAL_BRUSH_COUNT]; // Keeps track of brush cache
    POINTL      ptlReRealize;           // Work area for 864/964 pattern


    BYTE        NV_CursorImageData[32*32*2]; // Max size is 32x32  *2bytes/pixel
    BYTE        NV_TVCursorImageData[32*32*2]; // Max size is 32x32  *2bytes/pixel

    ////////// Text stuff:

    SURFOBJ*    psoText;                // 1bpp surface to which we will have
                                        //   GDI draw our glyphs for us
    /////////// DirectDraw stuff:

    ULONG       DDrawEnabledFlag;       // Flag denotes if DDRAW was enabled
    ULONG       ulMinOverlayStretch;    // Minimum stretch ratio for this mode,
    DWORD       hDdChannel;             // Channel handle
    DWORD       *ddChannelPtr;          // PIO channel ptr for ddraw
    DWORD       DummyReturn;            // Scratch area used to store return from RM fcts

#ifdef NVD3D
    /////////// Direct3D stuff:
    BOOL                        bD3DInitialized;
    DWORD                       lpLast3DSurfaceRendered;
    DWORD                       dwZBufferContextAddr;
    BOOL                        TwoDRenderingOccurred;
    BOOL                        ThreeDRenderingOccurred;
    struct _def_nv_d3d_context *FirstCtxt;
#endif // NVD3D
    BOOL                        bD3DappIsRunning;

    FNGRAPHICSENGINEBUSY*       pfnGraphicsEngineBusy;
    FNVBLANKISACTIVE*           pfnVBlankIsActive;
    FNWAITWHILEVBLANKACTIVE*    pfnWaitWhileVBlankActive;
    FNWAITWHILEDISPLAYACTIVE*   pfnWaitWhileDisplayActive;
    FNGETSCANLINE*              pfnGetScanline;
    FNFIFOISBUSY*               pfnFifoIsBusy;
    FNDDUPDATEFLIPSTATUS*       pfnUpdateFlipStatus;

    ////////// OpenGL ICD stuff:
    struct _OGLSERVER          *pOglServer;    // Pointer to structure containing OpenGL Server data.
    ULONG                      oglLastChannel; // Count of number of OGL clients connected to display driver
    ULONG                      dwGlobalModeSwitchCount;// Number of mode switches since boot
    ULONG                      OglStencilSwitch;       // Registry switch for OpenGL stencil
    ULONG                      oglColor32Depth16; // TRUE if hardware supports mixed 32/16 mode rendering
    ULONG                      oglColor16Depth32; // TRUE if hardware supports mixed 16/32 mode rendering
    ULONG                      oglMultisample;    // TRUE if hardware supports multisample rendering
    ULONG                      oglDDrawSurfaceCount;
    POINTER_LIST               oglDDrawSurfaceCountList;

    ULONG       iUniqueness;            // display uniqueness for tracking
                                        // resolution changes
    OH*         pohBackBuffer;          // Our 2-d heap allocation structure
                                        //   for the back-buffer
    ULONG       ulBackBuffer;           // Byte offset in the frame buffer
                                        //   to the start of the back-buffer
    OH*         pohRed;
    OH*         pohBlue;
    OH*         pohGreen;
    ULONG       SolidTextureOffset;     // Offset to solid texture data

    /////////// NV list of supported classes
    ULONG       nvNumClasses;
    ULONG*      nvClassList;

    CLASSOBJ    CurrentClass;          

    /////////// NV Buffer Management:

    ULONG       CurrentZOffset;
    ULONG       CurrentZPitch;
    ULONG       CurrentSourceOffset;
    ULONG       CurrentSourcePitch;
    ULONG       CurrentDestOffset;
    ULONG       CurrentDestPitch;
    ULONG       CurrentDitherState;
    ULONG       CurrentDisplayOffset;

    /////////// Ddraw Buffer Management:
    ULONG       DdCurrentSourceOffset;
    ULONG       DdCurrentSourcePitch;
    ULONG       DdCurrentDestOffset;
    ULONG       DdCurrentDestPitch;
    ULONG       DdClipResetFlag;

    // Vertex Cache Stuff

    ULONG       VRotIndex0[8];
    ULONG       VRotIndex1[8];
    ULONG       VRotIndex2[8];
    ULONG       VRotMask[8];
    ULONG       VCache[16];

    ULONG       SavedCursorCheckSums[NUM_CACHED_CURSORS];
    ULONG       SavedCursorBitmaps[NUM_CACHED_CURSORS][512];
    ULONG       NextCachedCursor;


    //Texture buffers related data
    ULONG       pListDevTex;            // A pointer to the list of device textures
                                        // being handled by the physical device

    ULONG       lastRenderedTexKey;     // Contains a reference to the texture (if any)
                                        // used in the last hw primitive rendering,
                                        // this helps us to avoid corrupting the texture
                                        // while it still may be in use by the hw

    NV_VIDEO_CLUTDATA  *ajClutData;     // Local copy of palette data
    /////////// NV Brush stuff:

    DWORD       AlignedBrush[8*8];      // Enough room for an 8x8 pattern

// NVTEST
    DWORD       NVAbsoluteLimitFlag;    // Use to calculate theoretical limit
    DWORD       NVStubTest;             // Used to return without doing anything..
    DWORD       TestChannelFlag;        // Used to return without doing anything..
    DWORD*      TestChannelPtr;
    DWORD       NVClipResetFlag;
    DWORD       NVPatternResetFlag;
    USHORT      NVFreeCount;
    DWORD       NVDebugTrace;           // Used to return without doing anything..
    DWORD       NVRecordFunctionsFlag;
    DWORD       NVLocation[70];        //Record iterations thru paths

    DWORD       dwMaxClip;              // Max. Clip value: (ppdev->cyMemory)<<16) | (0x7fff)

    //*********************************************************************************
    // Direct draw support.
    //*********************************************************************************

    // TBD: these will be replaced by vpp components in GLOBALDATA eventually. -@mjl@
    //    At present, they are used in nv4vid (among others perhaps)
#define NV_MAX_OVERLAY_SURFACES 10 // replaced by NV_VPP_MAX...
    DWORD                       dwOverlayEnabled;
    DWORD                       dwOverlayFormat;
    DWORD                       dwOverlaySurfaces;
    DWORD                       dwOverlayOwner;
    DWORD                       dwOverlaySurfaceLCL[NV_MAX_OVERLAY_SURFACES];
    DWORD                       dwOverlaySrcWidth;
    DWORD                       dwOverlaySrcHeight;
    DWORD                       dwOverlayDstWidth;
    DWORD                       dwOverlayDstHeight;
    DWORD                       dwOverlayDeltaX;
    DWORD                       dwOverlayDeltaY;
    DWORD                       dwOverlayDstX;        // overlay Dst starting point
    DWORD                       dwOverlayDstY;
    DWORD                       dwVideoColorKey;
    ULONG                       OverlaySrcX;        // overlay source starting point
    ULONG                       OverlaySrcY;
    ULONG                       OverlaySrcOffset;
    ULONG                       OverlaySrcPitch;
    ULONG                       OverlaySrcSize;
    ULONG                       OverlayBufferIndex;

    DWORD                       DDrawVideoSurfaceCount;
    ULONG                       NvFloating0UYVYSurfaceMem;
    ULONG                       NvFloating1UYVYSurfaceMem;
    ULONG                       NvFloating0YUYVSurfaceMem;
    ULONG                       NvFloating1YUYVSurfaceMem;
    ULONG                       NvFloatingMem2MemFormatBaseAddress;
    ULONG                       NvYUY2Surface0Mem;
    ULONG                       NvYUY2Surface1Mem;
    ULONG                       NvFloatingMem2MemFormatMemoryHandle;
    ULONG                       NvFloatingMem2MemFormatNextAddress;
    ULONG                       NvYUY2Surface2Mem;
    ULONG                       BaseAddress;
    PVOID                       NvDmaBufferFlat;

    //
    // Directdraw callbacks
    //
    DD_CALLBACKS            ddCallBacks;
    DD_SURFACECALLBACKS     ddSurfaceCallBacks;
    DD_PALETTECALLBACKS     ddPaletteCallBacks;

    GLOBALDATA              *pDriverData; // ->Global data used by DX6 driver.

#ifdef NVD3D
    //********************************************************************************
    // NT5 Specific data.
    //********************************************************************************
    PULONG                  dmaD3DPusherNotifier;
    BOOLEAN                 bDXGblStateInitialized;
#else
    BOOLEAN                 bDDChannelActive;
#endif // #ifdef NVD3D

    //
    // Cached surface pitch and offset for ddraw surfaces. This is for NV4 only, since
    // the NV4 ddraw patch runs in a seperate channel from the GDI driver.
    //
    ULONG   ddSrcOffset;
    ULONG   ddDstOffset;
    ULONG   ddSrcPitch;
    ULONG   ddDstPitch;

#ifdef VPE_NT
    //
    // VPE stuff
    //
    ULONG       ulCaps;
#endif

    PNV_NOTIFIERS               Notifiers;
/*#if (NVARCH < 0x4)
    // BUGBUG - Hack 'o rama, these should be NvNotification structs!
    BYTE                       NvVideoLutCursorDacNotifier[16*9];
    BYTE                       NvSyncNotifierFlat[16*5];
    BYTE                       NvFlipPrimaryNotifierFlat[16*9];
    BYTE                       NvFlipOverlayNotifierFlat[16*5];
    BYTE                       NvFlipVportNotifierFlat[16*5];
    BYTE                       NvDmaToMemNotifierFlat[16*5];
    BYTE                       NvDmaFromMemNotifierFlat[16*5];
    BYTE                        NvScaledImageNotifier[16*5];
    BYTE                        NvPrimary2dNotifier[16*5];
#endif
*/  ULONG                       fourCC[10];
    ULONG                       fNvActiveFloatingContexts;
    ULONG                       CurrentVisibleSurfaceAddress;
    DD_HALINFO                  HALInfo;
//    ULONG                       ddClipUpdate;
    BOOLEAN                     bHwVidOvl;
    ULONG                       dDrawSpareSubchannelObject;
    PVOID                       NvScanlineBufferFlat;
    ULONG                       NvDmaFromMemSurfaceMem;
    ULONG                       bEnableIF09;

    // jsw: global handles for the new architecture
    ULONG                       hClient;
    ULONG                       hDevice;
    ULONG                       hDevClass;
    ULONG                       ulDeviceReference;
#ifdef VPE_NT
    ULONG                       hVPEChannel;
    void*                       pMySurfaces;
#endif
    ULONG                       vppChannelNdx;
    ULONG                       hPioChannel;
    ULONG                       hDmaChannel;
    ULONG                       hFrameBuffer;
    ULONG                       hCursor;
    ULONG                       ulSurfaceAlign;     // hw required surface alignment
    ULONG                       ulSurfaceBitAlign;  // hw required surface alignment in term of bits
    LONG                        cxSurfaceMax;       // maximum surface width in pixels (based upon the current CONTEXT_SURFACES_2D pitch constraint)


#if (_WIN32_WINNT >= 0x0500) && !defined(NV3)
    SURFACEALIGNMENT            AgpHeapAlignment;
    LPVIDEOMEMORY               AgpHeap;
    LONG                        AgpDummyPitch;
    PVOID                       AgpPushBuffer;
    PVOID                       AgpHeapBase;
    BOOLEAN                     bAgp;               // Is AGP supported on this config?

#endif // #if _WIN32_WINNT > 0x0500

    ULONG                       ulEnableDualView;   // WINXP DualView
    ULONG                       ulDualViewSecondary;

    // used by NVHEAP_ macros
    ULONG   VideoHeapTotal;
    ULONG   VideoHeapFree;
    ULONG   cbGdiHeap;

#if _WIN32_WINNT >= 0x0500
    NV_SYSMEM_INFO  SysMemInfo;
    NV_OS_VERSION_INFO OsVersionInfo;
#endif // _WIN32_WINNT >= 0x0500

#ifndef NV3
    USHORT  dwDeviceVersion;
#endif // !NV3

    ULONG ulWorkAroundHwFlag;
    
    // VIRTUAL DESKTOP SUPPORT
        // Display driver needs 1+2*NV_NO_DACS from control panel to set the multi-view
        // 1. cxScreen, cyScreen is the big Virtual Desktop to GDI
        // 2. rclDisplayDesktop is the virtual desktop for each view fram each DAC
        // 3. width and height are calculated from rclCurrentDisplay which is the resolution 
        //    for setting mode for each DAC.  rclCurrentDisplay.top and rclCurrentDisplay.left
        //    are the intial display of each view.

    ULONG   ulDesktopMode;                      // 0x0000 -> Single Monitor
    ULONG   ulDriverNotSupportPanScan;          // Driver does not support Panning and Scanning
    ULONG   ulPuntCursorToGDI;                  // Win2K only punt the cursor back to GDI
    ULONG   ulNumberDacsOnBoard;                // Number of DACs on the board
    ULONG   ulNumberDacsConnected;              // Number of DACs which have a device (CRT/DFP/TV) connected to them
    ULONG   ulNumberDacsActive;                 // Number of DACs which are currently active. For example, in the Normal
                                                // mode, this value will be 1.
    ULONG   ulPrimaryBufferIndex;               // used by ddraw flip
    ULONG   ulDacStatus[NV_NO_DACS];            // The status of each dac (onBoard/connected/Active). Defined in nvMultiMon.h.
    ULONG   ulMoveDisplayDx;                    // Move Delta X to right or left direction
    ULONG   ulMoveDisplayDy;                    // Move Delta Y to up or down direction
    ULONG   ulGDIRefreshRate;                   // Refresh rate of the current mode as requested by GDI.
    ULONG   ulRefreshRate[NV_NO_DACS];          // Refresh rate in Hz for each head.
    RECTL   rclDisplayDesktop[NV_NO_DACS];      // Boundary of desktop;
    RECTL   rclCurrentDisplay[NV_NO_DACS];      // Current display view
    ULONG   ulHeadDisplayOffset[NV_NO_DACS];    // The offset calculated from rclDisplayDesktop (top, left and ppdev->lDelta)
    ULONG   ulFlipBase;                         // Multi buffer switching for 3D rendering
    ULONG   ulDeviceDisplay[NV_NO_DACS];        // Dynamically switch device; ulDeviceDisplay[0] will be primary; head 0 or head 1
    ULONG   ulEnableDDC[NV_NO_DACS];            // Enable EDID filtering by the display driver.
    ULONG   ulTimingOverRide[NV_NO_DACS];       // DMT/GTF/AUTO Timing override
    ULONG   ulEnablePanScan[NV_NO_DACS];        // Enable the virtual desktop (pan-scan).
    ULONG   ulFreezePanScan[NV_NO_DACS];        // Freeze the pan-scan.
    ULONG   ulFreezeOriginX[NV_NO_DACS];       // The user specified upper left corner for freezing the pan-scan.
    ULONG   ulFreezeOriginY[NV_NO_DACS];       // The user specified upper left corner for freezing the pan-scan.

    ULONG   ulDeviceMask[NV_NO_DACS];             // The bit mask ID of the device. Part of the new Head API of ResMan.
    ULONG   ulAllDeviceMask;                     // The bit mask for all the output device connectors in the board.
    ULONG   ulConnectedDeviceMask;               // The bit mask for all the currently connected output devices in the system.

    ULONG   ulDeviceType[NV_NO_DACS];           // The type of device connected to the head viz., CRT, TV, FlatPanel
                                                // defined in nvcm.h
    ULONG   ulHeadDeviceOptions[NV_NO_DACS];    // A matrix of possible device options for each head. Defined in nvMultiMon.h.
                                                // This info is used mainly by the NV control panel and gets scanned in after each
                                                // modeset since this matrix can be changed by the RM after a modeset.
                                                // This is a static entity as opposed to ulDeviceType[NV_NO_DACS] which specifies
                                                // the currently selected device type for this head.
    ULONG   ulTVFormat[NV_NO_DACS];             // The TV type: defined in nvcm.h
    ULONG   ulNumPhysModes[NV_NO_DACS];         // number of EDID trimmed, FB size trimmed and PixelClockFrequency trimmed modes
    ULONG   ulNumVirtualModes[NV_NO_DACS];      // number of FB size trimmed and PixelClockFrequency trimmed modes
    MODE_ENTRY *PhysModeList[NV_NO_DACS];       // EDID trimmed, FB size trimmed and PixelClockFrequency trimmed mode list.
    MODE_ENTRY *VirtualModeList[NV_NO_DACS];    // FB size trimmed and PixelClockFrequency trimmed mode list
    
    ULONG   TwinView_State;                     // normal, multi-mon or clone mode. Defined in nvMultiMon.h
    ULONG   TwinView_Orientation;               // horizontal or vertical. Only for multimon. Defined in nvMultiMon.h

    ULONG   ulPrimarySurfaceOffset;

    BOOL    bNVInitDmaCompleted;                // Make sure DMA initialization is completed.
    ULONG   Mobile;                             // Read from the registry. 0: Desktop, 1: Dell laptop, 2: Toshiba laptop.
                                                // NOTE: Display driver should not use this registry key. This will
                                                // be deleted eventually.
    ULONG   UseHWSelectedDevice;                // BIOS devices take precedence over windows devices.
    ULONG   UseHotKeySameMode;                  // On hot key switches, keeps the same desktop mode (resolution,depth).
                                                // This needs PAN_SCAN_SELECTION_ENABLE_ALL to have been turned on.
                                                // This field is obsolete now since no OEM is using this feature curently.
    ULONG   PanScanSelection;                   // absent or 0: pan-scan in only secondary monitor of clone mode: PAN_SCAN_SELECTION_CLONE_SECONDARY_ONLY
                                                // 1: Enable pan-scan in all modes: PAN_SCAN_SELECTION_ENABLE_ALL
                                                // 2: Disable pan-scan in all modes:PAN_SCAN_SELECTION_DISABLE_ALL 
    ULONG   GDIModePruning;                     // Absent or 0: Driver always provides EDID mode pruning protection.
                                                // 1:   For desktop or laptop systems, for CRT, for the
                                                //      standard mode device and the primary device in clone mode and
                                                //      both devices in XP DualView mode, lets the GDI handle EDID 
                                                //      mode pruning via "Hide Modes" monitor checkbox.
                                                // 2:   For desktop systems only, for CRT or DFP, for the
                                                //      standard mode device and the primary device in clone mode and
                                                //      both devices in XP DualView mode, lets the GDI handle EDID 
                                                //      mode pruning via "Hide Modes" monitor checkbox.
                                                //
    ULONG   EnableModesetStopImage;             // Sony specific feature to STOP_IMAGE during modesets and 
                                                // DPMS power on.
    ULONG   ulInduceModeChangeDeviceMask;       // The device mask/masks used by the NVSVC to induce mode change.
    ULONG   ulNVSVCStatus;                      // Started, stopped or paused status of the NVSVC service.
    ULONG   ACPISystem;                         // TRUE if we are on a ACPI system (such as laptops).

    ULONG   ulDFPSupports1400x1050;             // For Toshiba. Export 1400x1050 only on the 14x10 flat panel laptops.
    ULONG   ulDrvAssertModeTRUE;                // 1 if DrvAssertMode(TRUE). 0 if DrvEnableSurface().
    ULONG   ulDefaultTVFormat;                  // The latest and correct BIOS or user specified TV format.
    ULONG   ulDefaultTVDeviceType;              // PAL or NTSC device type based on the ppdev->ulDefaultTVFormat above.
    NVTWINVIEWDATA SaveSettings[NUM_ENTRIES_SAVE_SETTINGS]; // Saved settings for each device combo.
    // VIRTUAL DESKTOP SUPPORT

    ULONG   ulSaveScreenBitsRefCounter;                // is incremented per DrvSaveScreenBits call and used as unique ID.
    struct _LINKEDLISTHEAD *pLinkedListSaveScreenBits; // linked list of data collected in DrvSaveScreenBits

#if defined(_WIN64)
    PVOID fpState;                              // EngSaveFloatingPointState ptr
    ULONG fpStateSize;                          // EngSaveFloatingPointState len
#endif
#ifdef NT4_MULTI_DEV
    struct _MDEV *pmdev;                        // NT4 multiboarddescriptor for fast onedevice access        
#endif
    ULONG ulDriverCompatibilityMode;            // The video card's acceleration level (can be dynamically set through the Display Applet)
                                                // Value Description (W2K DDK):
                                                //     0 All display driver accelerations are permitted. 
                                                //     1 DrvSetPointerShape and DrvCreateDeviceBitmap are disabled. 
                                                //     2 In addition to 1, more sophisticated display driver accelerations are disallowed, including DrvStretchBlt, DrvFillPath, DrvGradientFill, DrvLineTo, DrvAlphaBlend, and DrvTransparentBlt. 
                                                //     3 In addition to 2, all DirectDraw and Direct3D accelerations are disallowed. 
                                                //     4 In addition to 3, almost all display driver accelerations are disallowed, except for solid color fills, DrvCopyBits, DrvTextOut, and DrvStrokePath. DrvEscape is disabled. 
                                                //     5 No hardware accelerations are allowed. The driver will only be called to do bit-block transfers from a system memory surface to the screen. 
    BOOL  bIsSWPointer;                         // NOTE: is true for SW emulated pointers (different to ulPuntCursorToGDI)
} PDEV, *PPDEV;


    // This is defined in the NT5 ddk, but nt4 needs it also
#if _WIN32_WINNT < 0x0500
    // Gamma support ( single head )
    typedef struct _GAMMARAMP
    {
      WORD Red[256];
      WORD Green[256];
      WORD Blue[256];
    }  GAMMARAMP, *PGAMMARAMP;

#endif

    // Gamma support ( multi head )
   typedef struct GAMMARAMP_MULTI
   {
      WORD        wHead; 
      GAMMARAMP   grGammaRamp;
   } GAMMARAMP_MULTI, *PGAMMARAMP_MULTI;


/////////////////////////////////////////////////////////////////////////
// Double Buffer Window Flipping Support

void NV_InitDoubleBufferMode(PDEV *ppdev, LONG NumBuffers, ULONG *MultiBufferOffsets);
void NV_DisableDoubleBufferMode(PDEV *ppdev);

/////////////////////////////////////////////////////////////////////////
// nv4hw.c export prototypes:

VOID NV4_DdPioSync(PDEV *ppdev);

/////////////////////////////////////////////////////////////////////////
// Miscellaneous prototypes:

BOOL __fastcall bIntersect(RECTL*, RECTL*, RECTL*);
LONG cIntersect(RECTL*, RECTL*, LONG);
DWORD getAvailableModes(HANDLE, PVIDEO_MODE_INFORMATION*, DWORD*, DWORD*);

BOOL bInitializeModeFields(PDEV*, GDIINFO*, DEVINFO*, DEVMODEW*);
BOOL bFastFill(PDEV*, LONG, POINTFIX*, ULONG, ULONG, RBRUSH*, POINTL*, RECTL*);

BOOL bEnableHardware(PDEV*);
VOID vDisableHardware(PDEV*);
BOOL bAssertModeHardware(PDEV*, BOOL);

void __cdecl NvSetDacImageOffset(PDEV* ppdev, ULONG ulDac, ULONG ulOffset);
BOOL __cdecl NvSetDac(PDEV *ppdev, ULONG ulHead, RESOLUTION_INFO * pResolution);
void __cdecl DisableHeadCursor(PDEV *ppdev, ULONG ulHead, ULONG ulDiable);
BOOL NvInitialDac(PDEV *ppdev);
VOID NvRestoreDacs(PDEV *ppdev);
VOID NvCreateObject(PDEV *, BOOL, PVOID, NvU32, NvU32 *, NvU32 *, int, NvU32 *);
BOOL NvGetSupportedClasses(PDEV *);

BOOL AllocDmaPushBuf(PDEV *ppdev);
VOID FreeDmaPushBuf(PDEV *ppdev);

BOOL AllocMemOncePerPdev(PDEV * ppdev);
VOID FreeMemOncePerPdev(PDEV * ppdev);

BOOL NvSetupContextDma(PDEV * ppdev);
VOID NvFreeContextDma(PDEV * ppdev);

BOOL NvSetupHwObjects(PDEV * ppdev);            
BOOL NvInitHwObjects(PDEV * ppdev);

ULONG OglSupportEnabled(PDEV *ppdev);
NV_OPENGL_CLIENT_INFO *OglFindClientInfoHWnd(PDEV *ppdev, HWND hWnd);
NV_OPENGL_CLIENT_INFO_LIST *OglFindClientInfoFromHWnd(PDEV *ppdev, HWND hWnd);

#if defined(_WIN64)
ULONG DrvEscape32(SURFOBJ *pso, ULONG iEsc,
                  ULONG cjIn, VOID *pvIn,
                  ULONG cjOut, VOID *pvOut);

VOID NvCopyStructIn (VOID *ptr32, VOID *ptr64, CHAR *fmt);
VOID NvCopyStructOut(VOID *ptr32, VOID *ptr64, CHAR *fmt);
#endif

extern BYTE gajHwMixFromMix[];
extern BYTE gaRop3FromMix[];
extern ULONG gaulHwMixFromRop2[];

/////////////////////////////////////////////////////////////////////////
// The x86 C compiler insists on making a divide and modulus operation
// into two DIVs, when it can in fact be done in one.  So we use this
// macro.
//
// Note: QUOTIENT_REMAINDER implicitly takes unsigned arguments.

#if defined(_X86_)

#define QUOTIENT_REMAINDER(ulNumerator, ulDenominator, ulQuotient, ulRemainder) \
{                                                               \
    __asm mov eax, ulNumerator                                  \
    __asm sub edx, edx                                          \
    __asm div ulDenominator                                     \
    __asm mov ulQuotient, eax                                   \
    __asm mov ulRemainder, edx                                  \
}

#else

#define QUOTIENT_REMAINDER(ulNumerator, ulDenominator, ulQuotient, ulRemainder) \
{                                                               \
    ulQuotient  = (ULONG) ulNumerator / (ULONG) ulDenominator;  \
    ulRemainder = (ULONG) ulNumerator % (ULONG) ulDenominator;  \
}

#endif

/////////////////////////////////////////////////////////////////////////
// OVERLAP - Returns TRUE if the same-size lower-right exclusive
//           rectangles defined by 'pptl' and 'prcl' overlap:

#define OVERLAP(prcl, pptl)                                             \
    (((prcl)->right  > (pptl)->x)                                   &&  \
     ((prcl)->bottom > (pptl)->y)                                   &&  \
     ((prcl)->left   < ((pptl)->x + (prcl)->right - (prcl)->left))  &&  \
     ((prcl)->top    < ((pptl)->y + (prcl)->bottom - (prcl)->top)))

/////////////////////////////////////////////////////////////////////////
// SWAP - Swaps the value of two variables, using a temporary variable

#define SWAP(a, b, tmp) { (tmp) = (a); (a) = (b); (b) = (tmp); }

//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
// CONVERT_TO_BYTES - Converts to byte count.

#define CONVERT_TO_BYTES(x, pdev)   ( (x) * pdev->cjPelSize)

//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
// CONVERT_FROM_BYTES - Converts to byte count.

#define CONVERT_FROM_BYTES(x, pdev) ( (x) / pdev->cjPelSize)

/////////////////////////////////////////////////////////////////////////
// Macro to determine if SURFOBJ is on primary screen
#define IS_SCREEN(pso)           (STYPE_DEVICE == pso->iType)
// Macro to determine if SURFOBJ any kind of devicebitmap
#define IS_DEVICEBITMAP(pso)     (STYPE_DEVBITMAP == pso->iType)
// Macro to determine if SURFOBJ a devicebitmap in device memory
#define IS_DEV_DEVICEBITMAP(pso) (IS_DEVICEBITMAP(pso) && (((DSURF *)pso->dhsurf)->dt == DT_SCREEN))
// Macro to determine if SURFOBJ a devicebitmap in host memory
#define IS_DIB_DEVICEBITMAP(pso) (IS_DEVICEBITMAP(pso) && (((DSURF *)pso->dhsurf)->dt == DT_DIB))
/////////////////////////////////////////////////////////////////////////
// Macro to determine if SURFOBJ is in framebuffer (device memory)
// NT4 doesn't have BMF_NOTSYSMEM or DrvDeriveSurface
#if _WIN32_WINNT >= 0x0500
#define IS_DEV_SURF(pso)   \
        (  IS_SCREEN(pso)  \
        || IS_DEV_DEVICEBITMAP(pso) \
        || (pso->fjBitmap & BMF_NOTSYSMEM) )
#else
#define IS_DEV_SURF(pso)   \
        (  IS_SCREEN(pso)  \
        || IS_DEV_DEVICEBITMAP(pso) )
#endif

/////////////////////////////////////////////////////////////////////////
// Macros used to access NV PRM regs.

#define PRMVIO_Base ppdev->PRMVIORegs
#define PRMCIO_Base ((PBYTE) (ppdev->PRMCIORegs))


//////////////////////////////////////////////////////////////////////

// These Mul prototypes are thunks for multi-board support:

ULONG   MulGetModes(HANDLE, ULONG, DEVMODEW*);
DHPDEV  MulEnablePDEV(DEVMODEW*, PWSTR, ULONG, HSURF*, ULONG, ULONG*,
                      ULONG, DEVINFO*, HDEV, PWSTR, HANDLE);
VOID    MulCompletePDEV(DHPDEV, HDEV);
HSURF   MulEnableSurface(DHPDEV);
BOOL    MulStrokePath(SURFOBJ*, PATHOBJ*, CLIPOBJ*, XFORMOBJ*, BRUSHOBJ*,
                      POINTL*, LINEATTRS*, MIX);
BOOL    MulFillPath(SURFOBJ*, PATHOBJ*, CLIPOBJ*, BRUSHOBJ*, POINTL*,
                    MIX, FLONG);
BOOL    MulBitBlt(SURFOBJ*, SURFOBJ*, SURFOBJ*, CLIPOBJ*, XLATEOBJ*,
                  RECTL*, POINTL*, POINTL*, BRUSHOBJ*, POINTL*, ROP4);
VOID    MulDisablePDEV(DHPDEV);
VOID    MulDisableSurface(DHPDEV);
BOOL    MulAssertMode(DHPDEV, BOOL);
VOID    MulMovePointer(SURFOBJ*, LONG, LONG, RECTL*);
ULONG   MulSetPointerShape(SURFOBJ*, SURFOBJ*, SURFOBJ*, XLATEOBJ*, LONG,
                           LONG, LONG, LONG, RECTL*, FLONG);
ULONG   MulDitherColor(DHPDEV, ULONG, ULONG, ULONG*);
BOOL    MulSetPalette(DHPDEV, PALOBJ*, FLONG, ULONG, ULONG);
BOOL    MulCopyBits(SURFOBJ*, SURFOBJ*, CLIPOBJ*, XLATEOBJ*, RECTL*, POINTL*);
BOOL    MulTextOut(SURFOBJ*, STROBJ*, FONTOBJ*, CLIPOBJ*, RECTL*, RECTL*,
                   BRUSHOBJ*, BRUSHOBJ*, POINTL*, MIX);
VOID    MulDestroyFont(FONTOBJ*);
BOOL    MulPaint(SURFOBJ*, CLIPOBJ*, BRUSHOBJ*, POINTL*, MIX);
BOOL    MulRealizeBrush(BRUSHOBJ*, SURFOBJ*, SURFOBJ*, SURFOBJ*, XLATEOBJ*,
                        ULONG);
HBITMAP MulCreateDeviceBitmap(DHPDEV, SIZEL, ULONG);
VOID    MulDeleteDeviceBitmap(DHSURF);
BOOL    MulStretchBlt(SURFOBJ*, SURFOBJ*, SURFOBJ*, CLIPOBJ*, XLATEOBJ*,
                      COLORADJUSTMENT*, POINTL*, RECTL*, RECTL*, POINTL*,
                      ULONG);

// These Dbg prototypes are thunks for debugging:

ULONG   DbgGetModes(HANDLE, ULONG, DEVMODEW*);
DHPDEV  DbgEnablePDEV(DEVMODEW*, PWSTR, ULONG, HSURF*, ULONG, ULONG*,
                      ULONG, DEVINFO*, HDEV, PWSTR, HANDLE);

VOID    DbgCompletePDEV(DHPDEV, HDEV);
VOID    DbgSynchronize(DHPDEV, RECTL *);
HSURF   DbgEnableSurface(DHPDEV);
BOOL    DbgLineTo(SURFOBJ*, CLIPOBJ*, BRUSHOBJ*, LONG, LONG, LONG, LONG,
                  RECTL*, MIX);
BOOL    DbgStrokePath(SURFOBJ*, PATHOBJ*, CLIPOBJ*, XFORMOBJ*, BRUSHOBJ*,
                      POINTL*, LINEATTRS*, MIX);
BOOL    DbgFillPath(SURFOBJ*, PATHOBJ*, CLIPOBJ*, BRUSHOBJ*, POINTL*,
                    MIX, FLONG);
BOOL    DbgBitBlt(SURFOBJ*, SURFOBJ*, SURFOBJ*, CLIPOBJ*, XLATEOBJ*,
                  RECTL*, POINTL*, POINTL*, BRUSHOBJ*, POINTL*, ROP4);
BOOL    DbgAlphaBlend(SURFOBJ*, SURFOBJ*, CLIPOBJ*, XLATEOBJ*, RECTL*,
                      RECTL*, BLENDOBJ*); 
BOOL    DbgTransparentBlt(SURFOBJ*, SURFOBJ*, CLIPOBJ*, XLATEOBJ*, RECTL*, RECTL*, ULONG, ULONG);
VOID    DbgDisablePDEV(DHPDEV);
VOID    DbgDisableSurface(DHPDEV);

BOOL    DbgAssertMode(DHPDEV, BOOL);

VOID    DbgMovePointer(SURFOBJ*, LONG, LONG, RECTL*);
ULONG   DbgSetPointerShape(SURFOBJ*, SURFOBJ*, SURFOBJ*, XLATEOBJ*, LONG,
                           LONG, LONG, LONG, RECTL*, FLONG);
ULONG   DbgDitherColor(DHPDEV, ULONG, ULONG, ULONG*);
BOOL    DbgSetPalette(DHPDEV, PALOBJ*, FLONG, ULONG, ULONG);
BOOL    DbgCopyBits(SURFOBJ*, SURFOBJ*, CLIPOBJ*, XLATEOBJ*, RECTL*, POINTL*);
BOOL    DbgTextOut(SURFOBJ*, STROBJ*, FONTOBJ*, CLIPOBJ*, RECTL*, RECTL*,
                   BRUSHOBJ*, BRUSHOBJ*, POINTL*, MIX);
VOID    DbgDestroyFont(FONTOBJ*);
BOOL    DbgPaint(SURFOBJ*, CLIPOBJ*, BRUSHOBJ*, POINTL*, MIX);
BOOL    DbgRealizeBrush(BRUSHOBJ*, SURFOBJ*, SURFOBJ*, SURFOBJ*, XLATEOBJ*,
                        ULONG);
HBITMAP DbgCreateDeviceBitmap(DHPDEV, SIZEL, ULONG);
VOID    DbgDeleteDeviceBitmap(DHSURF);
BOOL    DbgStretchBlt(SURFOBJ*, SURFOBJ*, SURFOBJ*, CLIPOBJ*, XLATEOBJ*,
                      COLORADJUSTMENT*, POINTL*, RECTL*, RECTL*, POINTL*,
                      ULONG);
ULONG   DbgEscape(SURFOBJ*, ULONG, ULONG, VOID*, ULONG, VOID*);
ULONG   DbgDrawEscape (SURFOBJ*, ULONG, CLIPOBJ*, RECTL*, ULONG, VOID*);
BOOL    DbgResetPDEV(DHPDEV, DHPDEV);

VOID NV3_SetSourceBase(PDEV *,ULONG,LONG);
VOID NV3_SetDestBase(PDEV *,ULONG,LONG);

VOID NvDDEnable(PDEV *ppdev);
NvU8 bCreateNVDDPatch(PDEV *ppdev);
NvU8 bDestroyNVDDPatch(PDEV *ppdev);

BOOL    DbgGetDirectDrawInfo(DHPDEV, DD_HALINFO*, DWORD*, VIDEOMEMORY*,
                             DWORD*, DWORD*);
BOOL    DbgEnableDirectDraw(DHPDEV, DD_CALLBACKS*, DD_SURFACECALLBACKS*,
                            DD_PALETTECALLBACKS*);
VOID    DbgDisableDirectDraw(DHPDEV);

#if _WIN32_WINNT >= 0x0500
BOOL    DbgIcmSetDeviceGammaRamp(DHPDEV,ULONG,LPVOID);
HBITMAP DbgDeriveSurface(DD_DIRECTDRAW_GLOBAL *, DD_SURFACE_LOCAL *);
void    DbgDrvNotify(SURFOBJ *, ULONG, PVOID);

#endif

#if DBG
    VOID    vAcquireCrtc(PDEV*);
    VOID    vReleaseCrtc(PDEV*);

    #define ACQUIRE_CRTC_CRITICAL_SECTION(ppdev)    vAcquireCrtc(ppdev)
    #define RELEASE_CRTC_CRITICAL_SECTION(ppdev)    vReleaseCrtc(ppdev)

#else

    /////////////////////////////////////////////////////////////////////////
    // Free Build
    //
    // For a free (non-debug build), we make everything in-line.

    // Safe port access macros -- these macros automatically do memory
    // -----------------------    barriers, so you don't have to worry
    //                            about them:

    // The CRTC register critical section must be acquired before
    // touching the CRTC register (because of async pointers):


    #define ACQUIRE_CRTC_CRITICAL_SECTION(ppdev)                \
        EngAcquireSemaphore(ppdev->csCrtc);

    #define RELEASE_CRTC_CRITICAL_SECTION(ppdev)                \
    {                                                           \
        EngReleaseSemaphore(ppdev->csCrtc);                     \
    }

#endif

//
// macro to access the hwGfxCaps which is initialized with NvConfigGet(..NV_CFG_GRAPHICS_CAPS..) (look nvcm.h)
//
#define HWGFXCAPS_MAXCLIPS(ppdev)       ((ppdev->hwGfxCaps >> NV_CFG_GRAPHICS_CAPS_MAXCLIPS_SHIFT) & NV_CFG_GRAPHICS_CAPS_MAXCLIPS_MASK)
#define HWGFXCAPS_QUADRO_GENERIC(ppdev) (ppdev->hwGfxCaps & NV_CFG_GRAPHICS_CAPS_QUADRO_GENERIC)
#define HWGFXCAPS_UBB(ppdev)            (ppdev->hwGfxCaps & NV_CFG_GRAPHICS_CAPS_UBB)


/*******************************************************************************
 *
 *    Useful information for OpenGL support.  Not guaranteed to work.
 *
 *******************************************************************************/


#define     OPENGL_CMD              4352
#define     OPENGL_GETINFO          4353
#define     NV_OPENGL_SET_ENABLE    0x6988
#define     OPENGL_GETINFO_DRVNAME  0

#define     OPENGL_ICD_VER          2
#define     OPENGL_DRIVER_VER       0x10000
#define     OPENGL_KEY_NAME_NV3     L"RIVA128"
#define     OPENGL_KEY_NAME_NV4     L"RIVATNT"

#define     ESC_ALLOC_CONTEXT_DMA                   0x6989
#define     ESC_FREE_CONTEXT_DMA                    0x698A
#define     ESC_ALLOC_CHANNEL_DMA                   0x698B
#define     ESC_FREE_CHANNEL_DMA                    0x698C
#define     ESC_DMA_FLOW_CONTROL                    0x698D
#define     ESC_SET_GAMMA_RAMP                      0x698E
#define     ESC_GET_GAMMA_RAMP                      0x698F
#define     ESC_SET_GAMMA_RAMP_MULTI                0x6990
#define     ESC_GET_GAMMA_RAMP_MULTI                0x6991
#define     ESC_GET_PIXEL_FORMAT                    0x7000
#define     ESC_NV_OPENGL_DMA_PUSH_GO               0x7001

#define DDI_VER_40 0x00020000

#define NV_DMA_SCANLINE_BUF_SIZE 32768

#ifndef NVD3D_DX6 // D3D driver conflicts
//*******************************************************************************
// Macros used to access NV registers.
//*******************************************************************************
#define REG_WR32(a,d)   ppdev->NvBaseAddr[(a)/4]=(U032)(d)
#define REG_RD32(a)     ppdev->NvBaseAddr[(a)/4]
#define REG_WR08(a,d)   ((V008 *)(ppdev->NvBaseAddr))[(a)]  =(U008)(d)
#define REG_RD08(a)     ((V008 *)(ppdev->NvBaseAddr))[(a)]

#define BIT(b)                  (1<<(b))
#define DEVICE_BASE(d)          (0?d)
#define DEVICE_EXTENT(d)        (1?d) - DEVICE_BASE(d) + 1
#define DRF_SHIFT(drf)          ((0?drf) % 32)
#define DRF_MASK(drf)           (0xFFFFFFFF>>(31-((1?drf) % 32)+((0?drf) % 32)))
#define DRF_DEF(d,r,f,c)        ((NV ## d ## r ## f ## c)<<DRF_SHIFT(NV ## d ## r ## f))
#define DRF_NUM(d,r,f,n)        (((n)&DRF_MASK(NV ## d ## r ## f))<<DRF_SHIFT(NV ## d ## r ## f))
#define DRF_VAL(d,r,f,v)        (((v)>>DRF_SHIFT(NV ## d ## r ## f))&DRF_MASK(NV ## d ## r ## f))
#define REG_WR_DRF_NUM(d,r,f,n) REG_WR32(NV ## d ## r, DRF_NUM(d,r,f,n))
#define REG_WR_DRF_DEF(d,r,f,c) REG_WR32(NV ## d ## r, DRF_DEF(d,r,f,c))
#define FLD_WR_DRF_NUM(d,r,f,n) REG_WR32(NV##d##r,(REG_RD32(NV##d##r)&~(DRF_MASK(NV##d##r##f)<<DRF_SHIFT(NV##d##r##f)))|DRF_NUM(d,r,f,n))
#define FLD_WR_DRF_DEF(d,r,f,c) REG_WR32(NV##d##r,(REG_RD32(NV##d##r)&~(DRF_MASK(NV##d##r##f)<<DRF_SHIFT(NV##d##r##f)))|DRF_DEF(d,r,f,c))
#define REG_RD_DRF(d,r,f)       (((REG_RD32(NV ## d ## r))>>DRF_SHIFT(NV ## d ## r ## f))&DRF_MASK(NV ## d ## r ## f))
#endif // #ifndef NVD3D_DX6

#ifdef __cplusplus
}
#endif // __cplusplus

