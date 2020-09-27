//**************************************************************************
//
//       Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.
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
//       Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.
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
//
//  Module: nvPatchInterface.h
//        nvPatchInterface specification to allow for shared ogl/d3d access
//
//  History:
//        Daniel Rohrer    (drohrer)      23Jun00      created
//
// **************************************************************************

// using generic C structs for shared library compatability with OGL.

#ifndef __NV_PATCH_INTERFACE_H
#define __NV_PATCH_INTERFACE_H

#include "nvUniversal.h"
#if !defined(IS_OPENGL)
    #define FASTCALL _fastcall
#else
    #include "os.h"
#endif

// FD mutual exclusion 
#define FD_SEMAPHORE_HANDLE                                     0xDDFD0000
#define FD_SEMAPHORE_206E_CONTEXT_DMA                           0xDDFD0001
#define FD_SEMAPHORE_097_CONTEXT_DMA                            0xDDFD0002
//          ... 1-32 should be considered used here for our channels ... 
//#define FD_SEMAPHORE_CONTEXT_DMA                              0xDDFD0020

/*
** Standard polymorphic data types that are not present in older header files.
*/
#if defined(_WIN32) && (_MSC_VER < 1300)

typedef int            intptr_t;
typedef unsigned int   uintptr_t;

#endif

/*****************************************************************************/
// Eval memory allocation. Avoid seperate malloc for each section of memory.
// allocate a big chunk with EV_ALLOC_INIT
// and dole it out efficiently with EV_ALLOC
// free it with EV_FREE
// Only one DECL per routine.
// Use EV_ALLOC_INIT/EV_FREE to alloc/free within a routine.

// To alloc something that will persist to parallel or higher routines,
// use EV_ALLOC_INIT_WITH_CACHE with a NV_PATCH_ALLOC_CACHE
// The higher level routines must free with EV_FREE_CACHE

// Use EV_ALLOC_RESET to use memory chunk allocated by higher level routine.
//
// Use EV_ROUND if necessary to ensure allocations have pointer sized
// granularity to avoid address exceptions on the IA64.

// used by cached-alloc algorithm to avoid allocs on each imm mode call
// Each call to EV_ALLOC_INIT_WITH_CACHE can specify a cache to use.
// The coder must be careful that a cache is not used twice at a time.
#if !defined(IS_OPENGL)
typedef struct {
        void *pBaseCache;
        int bytesAllocCache;
        int countFreeCheck;     // count times past a potential free point
        int inUse;              // for debug check
        void *contextCache;     // for debug check
} NV_PATCH_ALLOC_CACHE;
#define MAX_EV_CACHE                        10

#else
// for OpenGL, these are defined in eval.h
#endif

typedef struct {
        void *context;
        void *pBase;
        void *pAvail;
        void *pAllocatedBase;
        int bytesAlloc;
        int bytesUsed;
        NV_PATCH_ALLOC_CACHE *pCache; // ptr to higher level cache structure
} NV_PATCH_ALLOC_CONTROL;

#define EV_ALLOC_BYTE_ALIGN              0x10
#define EV_CACHE_INFO_SCRATCH               0
#define EV_CACHE_QUADINFO_ETC               1
#define EV_CACHE_9FDMATRICES                2
#define EV_CACHE_AUTO_NORMAL_SETUP          3
#define EV_CACHE_UV_SETUP                   4
#define EV_CACHE_TRI_WEIGHTS                5
#define EV_CACHE_RETESS0                    6
#define EV_CACHE_RETESS1                    7
#define EV_CACHE_DRAW_SW                    8
#define EV_CACHE_SWATHING                   9
// if you add more after '9', you must change MAX_EV_CACHE


#define EV_ALLOC_DECL() NV_PATCH_ALLOC_CONTROL evMemory = {NULL, 0}

#define EV_ALLOC_INIT_WITH_CACHE(CONTEXT, TBYTES, errorLabel, PCACHE) \
        if (evAllocInitWithCache(info, CONTEXT, &evMemory, TBYTES, PCACHE)) { \
evAllocError: \
            nvAssert(("EV_ALLOC_INIT FAILURE",0)); \
            info->retVal |= NV_PATCH_EVAL_ERR_OUT_OF_MEMORY; \
            goto errorLabel; \
        } \

#define EV_ALLOC_INIT_WITH_CACHE2(CONTEXT, TBYTES, errorLabel, PCACHE) \
        if (evAllocInitWithCache(info, CONTEXT, &evMemory, TBYTES, PCACHE)) { \
            nvAssert(("EV_ALLOC_INIT FAILURE",0)); \
            info->retVal |= NV_PATCH_EVAL_ERR_OUT_OF_MEMORY; \
            goto errorLabel; \
        } \

#define EV_ALLOC_INIT(CONTEXT, TBYTES, errorLabel) \
        EV_ALLOC_INIT_WITH_CACHE(CONTEXT, TBYTES, errorLabel, NULL)

#if defined(IS_OPENGL)
#define EV_ALLOC(VAR, CONTEXT, BYTES) {                             \
        int __nBytes;                                               \
        nvAssert(CONTEXT == evMemory.context);                      \
        nvAssert(!((BYTES) & (EV_ALLOC_BYTE_ALIGN - 1)));           \
        evMemory.bytesUsed += (__nBytes = (BYTES));                 \
        nvAssert(evMemory.bytesUsed <= evMemory.bytesAlloc);        \
        if (evMemory.bytesUsed <= evMemory.bytesAlloc) {            \
            *(void **)(&(VAR)) = evMemory.pAvail;                   \
            evMemory.pAvail = (char *)evMemory.pAvail + __nBytes;   \
            nvAssert(!((uintptr_t)evMemory.pAvail & ((sizeof(void *) - 1)))); \
        } else {                                                    \
            VAR = NULL;                                             \
        }                                                           \
        if (!VAR) goto evAllocError;                                \
    }
#else
#define EV_ALLOC(VAR, CONTEXT, BYTES) {                             \
        int __nBytes;                                               \
        nvAssert(!((BYTES) & (EV_ALLOC_BYTE_ALIGN - 1)));           \
        evMemory.bytesUsed += (__nBytes = (BYTES));                 \
        nvAssert(evMemory.bytesUsed <= evMemory.bytesAlloc);        \
        if (evMemory.bytesUsed <= evMemory.bytesAlloc) {            \
            *(void **)(&(VAR)) = evMemory.pAvail;                   \
            evMemory.pAvail = (char *)evMemory.pAvail + __nBytes;   \
            nvAssert(!((uintptr_t)evMemory.pAvail & ((sizeof(void *) - 1)))); \
        } else {                                                    \
            VAR = NULL;                                             \
        }                                                           \
        if (!VAR) goto evAllocError;                                \
    }
#endif

#define EV_ALLOC_FULL(CONTEXT) \
        /* expect equal if pre-calc was accurate */ \
        nvAssert(evMemory.bytesUsed == evMemory.bytesAlloc);

#define EV_FREE(VAR, CONTEXT) { \
        nvAssert(CONTEXT == evMemory.context); \
        if (evMemory.pBase && (VAR)) { \
            if (!evMemory.pCache) { \
                info->callbacks->free(CONTEXT, evMemory.pAllocatedBase); \
            } else { \
                evMemory.pCache->inUse = 0; \
            } \
            evMemory.pBase = NULL; \
            evMemory.pAllocatedBase = NULL; \
        } \
        VAR = NULL; \
    }

#define EV_FREE_CACHE(PCACHE, CONTEXT) { \
        if ((PCACHE)->pBaseCache) { \
            info->callbacks->free((PCACHE)->CONTEXT, (PCACHE)->pBaseCache); \
            (PCACHE)->pBaseCache = NULL; \
        } \
        (PCACHE)->inUse = 0; \
    } \

//#define EV_ROUND(BYTES) \
//    (((BYTES) + (sizeof(void *) - 1)) & ~(sizeof(void *) - 1))

//force 16 byte alignment...
#define EV_ROUND(BYTES) \
    (((BYTES) + (EV_ALLOC_BYTE_ALIGN - 1)) & ~(EV_ALLOC_BYTE_ALIGN - 1))

/*****************************************************************************/
#define NV_PATCH_ATTRIB_POSITION            0
#define NV_PATCH_ATTRIB_VERTEX_WEIGHT       1
#define NV_PATCH_ATTRIB_NORMAL              2
#define NV_PATCH_ATTRIB_COLOR               3
#define NV_PATCH_ATTRIB_SECONDARY_COLOR     4
#define NV_PATCH_ATTRIB_FOG_COORD           5
#define NV_PATCH_ATTRIB_UNUSED0             6
#define NV_PATCH_ATTRIB_UNUSED1             7
#define NV_PATCH_ATTRIB_TEXCOORD0           8
#define NV_PATCH_ATTRIB_TEXCOORD1           9
#define NV_PATCH_ATTRIB_TEXCOORD2           10
#define NV_PATCH_ATTRIB_TEXCOORD3           11
#define NV_PATCH_ATTRIB_TEXCOORD4           12
#define NV_PATCH_ATTRIB_TEXCOORD5           13
#define NV_PATCH_ATTRIB_TEXCOORD6           14
#define NV_PATCH_ATTRIB_TEXCOORD7           15

// Some of the ATTR bit names are used so much, and are so long, 
// they were making line lengths in excess of 140 characters.
// Rather than split the lines, these names have abbreviations
#define ATTR_V  NV_PATCH_ATTRIB_POSITION
#define ATTR_N  NV_PATCH_ATTRIB_NORMAL

//return codes for EvalPatch
#define NV_PATCH_EVAL_OK                 0x1
#define NV_PATCH_EVAL_ERR_OUT_OF_MEMORY  0x10
#define NV_PATCH_EVAL_ERR_NOT_HANDLED    0x20

// D3D, OGL, and Kelvin all use 16 separate vertex attributes.
#define NV_PATCH_NUMBER_OF_ATTRIBS      16

// D3D supports up to 5, OGL supports up to 8, and Kelvin supports up to 16.
// Note that when computing normal patches, an 8th-order nonrational patch
// becomes a 14th-order rational patch, and an 8th-order rational patch
// exceeds the Kelvin limit easily.
#define NV_PATCH_MAX_ORDER              16
#define NV_PATCH_MAX_RETESS             (1+NV_PATCH_MAX_ORDER)

// The maximum size of a swatch is determined by the size of the vertex
// file.   Henry says this is the magic number.  16 might work better, but
// it's not clear.
#define NV_PATCH_KELVIN_SWATCH_SIZE     17


#define NV_PATCH_MAX_TOTAL_STEP 1000    // total steps allowed
#define NV_PATCH_MAX_SWATCH_STEP 40     // max steps within a sub-patch

// When drawing stitch in SW modes, the buffer is this many vertices.
//  +1 because n steps means (n+1) vertices
//  +2 because may need to share 2 vertices between groups of n steps
#define NV_PATCH_MAX_SW_VERTICES (NV_PATCH_MAX_SWATCH_STEP+1+2)

#define EV_AS_INT(x)     (*((int *) &(x)))

typedef float NV_PATCH_CURVE_COEFFS[NV_PATCH_MAX_ORDER][4];
typedef double NV_PATCH_CURVE_COEFFS_D[NV_PATCH_MAX_ORDER][4];

// Matrix structure for matrix math
typedef struct {
    int rows, columns;
    int pad[2];
    NV_PATCH_CURVE_COEFFS data[NV_PATCH_MAX_ORDER];     // [row][col][xyzw]
} FDMatrix;

typedef struct {
    int rows, columns;
    int pad[2];
    NV_PATCH_CURVE_COEFFS_D data[NV_PATCH_MAX_ORDER];   // [row][col][xyzw]
} FDMatrixd;
#define FDMATRIX_HEADER_SIZE    4*sizeof(int)

// For swathing, an array of ptrs to FDMatrices is used to
// find a pointer to a particular FDMatrix given an attribute number
typedef FDMatrix *FDMatrixAttrSet[NV_PATCH_NUMBER_OF_ATTRIBS];

// Evaluated vertex attributes
typedef struct {
   float vertexAttribs[NV_PATCH_MAX_SW_VERTICES][NV_PATCH_NUMBER_OF_ATTRIBS][4];
} NV_PATCH_EVAL_OUTPUT;


// This is given to beginPrimitive to indicate what type of primitive to draw.
typedef enum {
    NV_PATCH_PRIMITIVE_TSTRIP = 1,
    NV_PATCH_PRIMITIVE_TFAN   = 2,
} NV_PATCH_PRIMITIVE_TYPE;

// Information for forward differencing one curve
typedef struct {
    int order;
    int pad[3];
    NV_PATCH_CURVE_COEFFS coeffs;
} NV_PATCH_CURVE_INFO;
#define PATCH_CURVE_HEADER_SIZE     4*sizeof(int)

// For swathing, an array of ptrs to NV_PATCH_CURVE_INFOs is used to
// find a pointer to a particular guard curve given an attribute number
typedef NV_PATCH_CURVE_INFO *FDCurveAttrSet[NV_PATCH_NUMBER_OF_ATTRIBS];

// Enumerant for what basis the patch is defined on
typedef enum {
    NV_PATCH_BASIS_BEZIER      = 0,
    NV_PATCH_BASIS_BSPLINE     = 1,
    NV_PATCH_BASIS_CATMULL_ROM = 2,
} NV_PATCH_BASIS_TYPE;

// Enumerant for the format of the control points
typedef enum {
    NV_PATCH_VERTEX_FORMAT_FLOAT_1 = 0, // Expand to (value,   0.0,   0.0, 1.0)
    NV_PATCH_VERTEX_FORMAT_FLOAT_2 = 1, // Expand to (value, value,   0.0, 1.0);
    NV_PATCH_VERTEX_FORMAT_FLOAT_3 = 2, // Expand to (value, value, value, 1.0);
    NV_PATCH_VERTEX_FORMAT_FLOAT_4 = 3, // No expansion required
    NV_PATCH_VERTEX_FORMAT_D3DCOLOR= 4, // Unpack bytes and remap to [0,1.0]
    NV_PATCH_VERTEX_FORMAT_UBYTE   = 5, // Unpack bytes and remap to [0,1.0]
    NV_PATCH_VERTEX_FORMAT_SHORT_2 = 5, // 2D signed short expanded to (value, value, 0., 1.)
    NV_PATCH_VERTEX_FORMAT_SHORT_4 = 7  // 4D signed short
} NV_PATCH_VERTEX_FORMAT_TYPE;

// Dirty flags for each map
#define NV_PATCH_DIRTY_NONE             0x00000000
#define NV_PATCH_DIRTY_CONTROL_POINTS   0x00000001
#define NV_PATCH_DIRTY_TESSELLATION     0x00000002

// Enumerant for the type of backend
typedef enum {
    NV_PATCH_BACKEND_IMMEDIATE = 1, // call immediately into a NV_PATCH_BACKEND
    NV_PATCH_BACKEND_CELSIUS   = 2, // write a Celsius pushbuffer
    NV_PATCH_BACKEND_KELVIN    = 3, // write a Kelvin pushbuffer
    NV_PATCH_BACKEND_DP2       = 4, // write a DP2 command stream
} NV_PATCH_BACKEND_TYPE;

// Guard information for a single subquad within a fractional tesselation
typedef struct {                                //     |              |
    NV_PATCH_CURVE_INFO *guardU0;               //     |              |center
    NV_PATCH_CURVE_INFO *guardV0;               // Vmid*---UCenter----*-----
    NV_PATCH_CURVE_INFO *guardUCenter;          //     |              |
    NV_PATCH_CURVE_INFO *guardVCenter;          //     |    _         |
    float *uMid;                                //     |   _/|      VCenter
    float *vMid;                                //    V0 _/           |
    float *center;                              //     |/             |
} NV_PATCH_FRAC_GUARD_INFO;                     //     +------U0------*------
                                                //                  Umid

// Guard information for a single attribute for a fractionally tesselated
// tensor product patch
typedef struct {                            //                    u1Mid
    NV_PATCH_CURVE_INFO guardU00;           //       *------U01---->*<-----U11-----*
    NV_PATCH_CURVE_INFO guardU01;           //       |\_            |            _/|
    NV_PATCH_CURVE_INFO guardU10;           //       |  \_          |          _/  |
    NV_PATCH_CURVE_INFO guardU11;           //      V01  _\| 01    VC1    11 |/_  V11
    NV_PATCH_CURVE_INFO guardV00;           //       |              |              |
    NV_PATCH_CURVE_INFO guardV01;           //       v              vcenter        v
    NV_PATCH_CURVE_INFO guardV10;           //  v0Mid*------UC0---->*<-----UC1-----* v1Mid
    NV_PATCH_CURVE_INFO guardV11;           //       ^              ^              ^
    NV_PATCH_CURVE_INFO guardUCenter0;      //       |    _         |         _    |
    NV_PATCH_CURVE_INFO guardUCenter1;      //      V00  _/|  00   VC0    10 |\_  V10
    NV_PATCH_CURVE_INFO guardVCenter0;      //       | _/           |           \_ |
    NV_PATCH_CURVE_INFO guardVCenter1;      //       |/             |             \|
    float u0Mid[4], v0Mid[4], u1Mid[4];     //       *------U00---->*<-----U10-----*
    float v1Mid[4], center[4];              //                    u0Mid
} NV_PATCH_FRAC_QUAD_GUARD_INFO;


// Guard information for a single attribute for a fractionally tesselated
// triangular patch
typedef struct {                            //                +                -
    NV_PATCH_CURVE_INFO guard[3][2];        //               / \               -
    NV_PATCH_CURVE_INFO guardCenter[3];     //              /   \              -
    float mid[3][4];                        //             /     \             -
    float center[4];                        //           g20     g11           -
} NV_PATCH_FRAC_TRI_GUARD_INFO;             //           /         \           -
                                            //          /           \          -
                                            //    mid? +             + mid?    -
                                            //        / \_         _/ \        -
                                            //       /    gc?    gc?   \       -
                                            //      /       \_ _/       \      -
                                            //    g21         + center  g10    -
                                            //    /           |           \    -
                                            //   /           gc?           \   -
                                            //  /             |             \  -
                                            // +------g00-----+------g01-----+ -
                                            //               mid?



// Illustration of quadInfo information.
// Used by all patch styles at the low level swathing and drawing routines
// This is a connonical view assuming that uDir=1 and vDir=1
// The U coordinates are reversed if uDir=0
// The V coordinates are reversed if vDir=0
// Also, pUInner and pVInner may be on the other side of the quad.
//
//     c01                     c11
//      *--------pU1------------*
//      |                    __/|
//      |                   /   |
//      +-----pUInner-------+   |
//      |                   |   |
//      |                   |   |
//      |                   |   |
//     pV0             pVInner pV1
//      |                   |   |
//      |                   |   |
//      |                   |   |
//      |                   |   |
//      |                   |   |
//      +--------pU0--------+---*
//     c00                 pVB c10



// This fake picture of quadInfo below shows transitions on all sides.
// This can never happen, but serves to illustrate the meaning of the
// stitchUBegin/End and stitchVBegin/End values.
// These may be slightly counterintuitive, but that depends on who you are.
// The thing to note is that the regions specified by quadInfo->stitchU* 
// are between curves specified with V. U regions and V curves mix.
// Again, like above, the coordinates of U and V may be reversed if
// the u/vDir is flipped.

//                                 __ quadInfo->stitchVEnd
//                                /
//               c01             /       c11
//                *--------pU1--/---------*  <-- ppGuardSetUEnd at top of regular grid
//                |\__         o       __/|
//                |   \               /   |
// quadInfo->     |   +-pUInnerT------+   |  <-- ppGuardSetUEnd is here if top stitch, quadInfo->pSwatchUEnd for "top" of swatch
//   stitchUBegin---o |               |   |
//                |   pVInnerL        | o---- quadInfo->stitchUEnd
//                |   |               |   |
//               pV0  |        pVInnerR  pV1
//                |   |               |   |
//                |   |               |   |
//                |   +-pUInnerB------+   |  <-- ppGuardSetUBegin is here if bot stitch, quadInfo->pSwatchUBegin for "bot" of swatch
//                | __/         o     \__ |
//                |/             \       \|
//                +--------pU0----\-------*  <-- ppGuardSetUBegin at bot of regular grid
//               c00               \     c10
//                                  \
//                                   -- quadInfo->stitchVBegin

// Following statements use U, but analogously apply equally to V
// ppGuardSetUBegin/UEnd are for the boundaries of the regular grid.
// ppGuardSetUBegin/UEnd may be either pU0, pUInnerL, pUInnerR, or pU1
// see "BEGIN SIMILAR CODE" in nvpatchutil.c
// Some cases are not so obvious, like when the height of the quad
// is only one segment, and there's a top stitch, then
// both ppGuardSetUBegin and UEnd are set to pU0.
// If height is 1, and bot stitch, then both are set to pU1.
// This simplifies other code which must pick off values at the edges of the regular grid.
// Also, ppStitchUInner may be either pU0 or pU1 in degenerate cases.

//
typedef struct {
    // corners of the original control points
    float corners[2][2][4]; // [v][u][xyzw]
} NV_PATCH_CORNER_INFO;

// Common information for any quad drawn by the lower level swathing and drawing code.
// This is for INT/FRAC and QUAD/TRI patches.
typedef struct {
    int nu0, nu1, nv0, nv1;                     // actual number of segments along each edge
    int uMaxSegs, vMaxSegs, uMinSegs, vMinSegs; // MIN and MAX comparing nu0:nu1 and nv0:nv1
    int needUInner, needVInner;                 // set if U/V Inner guards required

    // flags for transitions required.
    // Names refer to original LRTB, not flipped versions or anything
    // HW must render INT and FRAC stitches in order TOP/RIGHT/BOTTOM/LEFT
    int stitchTop, stitchRight, stitchBottom, stitchLeft;

    // These begin/end's mean in the direction of U (L/R) and V (B/T)
    // note: you may find these names confusing, since they don't correspond
    // to U0, U1. In other words, stitchVBegin means U0/UInner create a stitch
    // (in the canonical view).
    // But, we decided to leave things well enough alone.
    int stitchUBegin, stitchUEnd, stitchVBegin, stitchVEnd;

    int u0Dir, v0Dir, u1Dir, v1Dir; // '1' means normal direction (For u this is L-->R, for v this is B-->T)
    int oldu0Dir, oldv0Dir, oldu1Dir, oldv1Dir;

    // MaxDir are directions of regular grid
    int uMaxDir, vMaxDir;
    int olduMaxDir, oldvMaxDir;

    float du0, du1, dv0, dv1, duMax, dvMax; // small step size along edges, and for regular grid

    FDCurveAttrSet pU0;
    FDCurveAttrSet pU1;
    FDCurveAttrSet pV0;
    FDCurveAttrSet pV1;
    FDCurveAttrSet pUInner;
    FDCurveAttrSet pVInner;

    NV_PATCH_CORNER_INFO cornAttr[NV_PATCH_NUMBER_OF_ATTRIBS];      // general guard info
    float *pCorners[NV_PATCH_NUMBER_OF_ATTRIBS][2][2];

    // ptrs to set of edge curves for a given swatch
    FDCurveAttrSet *pSwatchUBegin, *pSwatchUEnd;
    FDCurveAttrSet *pSwatchVBegin, *pSwatchVEnd;

    FDCurveAttrSet *pCurvesTop, *pCurvesBot;
    NV_PATCH_EVAL_OUTPUT *pOut1, *pOut2;

    // CACHED DATA
    // the driver should allocate memory for these data members BEFORE
    // calling into the shared code.  Shared code should not be responsible
    // for the life time of these data members.
    FDMatrix *m00[NV_PATCH_NUMBER_OF_ATTRIBS], *m01[NV_PATCH_NUMBER_OF_ATTRIBS],
             *m10[NV_PATCH_NUMBER_OF_ATTRIBS], *m11[NV_PATCH_NUMBER_OF_ATTRIBS];    //[attribute]  (sub-patch index = 0 for bezier types)

    //we should probably union these....
    NV_PATCH_FRAC_QUAD_GUARD_INFO *guardQF[NV_PATCH_NUMBER_OF_ATTRIBS];             //[attribute]  (sub-patch index = 0 bezier types)
    NV_PATCH_FRAC_TRI_GUARD_INFO  *guardTF[NV_PATCH_NUMBER_OF_ATTRIBS];
} NV_PATCH_QUAD_INFO;

// for quadInfo->setGridCorner
#define NV_PATCH_SET_GRID_CORNER00      0x1
#define NV_PATCH_SET_GRID_CORNER10      0x2
#define NV_PATCH_SET_GRID_CORNER01      0x4
#define NV_PATCH_SET_GRID_CORNER11      0x8
#define NV_PATCH_SET_GRID_CORNER_ALL    0xf

// Information for one map within the current patch
typedef struct {
    unsigned int uorder, vorder;
    float *rawData;
    int stride;         // OGL: stride == typesize*sizeof(float)
    int pitch;          // OGL: pitch == uorder, number of coords
    int ufStride;       // u stride in floats (i.e. 4 floats is 4, not 16)
    int vfStride;       // v stride in floats
    NV_PATCH_VERTEX_FORMAT_TYPE maptype;
    NV_PATCH_VERTEX_FORMAT_TYPE Originaltype;
    unsigned int dirtyFlags;          // Currently ignored
} NV_PATCH_MAP_INFO;

// Declaration of an immediate-mode-style backend
typedef struct {    
    void (*InitBackend)(void *info);
    void (*DestroyBackend)(void *info);
    void (*beginPrimitive)(void *info, NV_PATCH_PRIMITIVE_TYPE primType);
    void (*endPrimitive)(void *info);
    void (*sendVertex)(void *info, NV_PATCH_EVAL_OUTPUT *pData, int index);
    void (*setFrontFace)(void *info, int reversed);
    void (*beginPatch)(void *info);
    void (*endPatch)(void *info);
} NV_PATCH_BACKEND;

// These are the callbacks that the shared code needs access to.  It is NOT SAFE
// to call C standard library or Windows API functions from the shared code!
typedef struct {
    void *(*malloc)(void *context, size_t bytes);
    void (*free)(void *context, void *ptr);
    void (*memcpy)(void *dst, const void *src, size_t bytes);
    void (*flushOutput)(void *context, unsigned int bytesWritten);
} NV_PATCH_DRIVER_CALLBACKS;

#if defined(IS_OPENGL)
  #define __NV_MALLOC(TAG, SIZE)        __GL_MALLOC(TAG, SIZE)
  #define __NV_FREE(TAG, ADDR)          __GL_FREE(TAG, ADDR)
  #define __NV_MEMCPY(DST, SRC, BYTES)  __GL_MEMCOPY(DST, SRC, BYTES)
#else
  #define __NV_MALLOC(TAG, SIZE)        info->callbacks->malloc(TAG, SIZE)
  #define __NV_FREE(TAG, ADDR)          info->callbacks->free(TAG, ADDR)
  #define __NV_MEMCPY(DST, SRC, BYTES)  info->callbacks->memcpy(DST, SRC, BYTES)
#endif

/*****************************************************************************/

// ***** bits for info->swatchFlags
#define NV_PATCH_SWATCH                 0x00000001      // set when swathing
#define NV_PATCH_SWATCH_FLIPUV          0x00000002      // for tri frac

#define NV_PATCH_SWATCH_FIRST_ROW       0x00000100
#define NV_PATCH_SWATCH_LAST_ROW        0x00000200
#define NV_PATCH_SWATCH_FIRST_COL       0x00000400
#define NV_PATCH_SWATCH_LAST_COL        0x00000800

#define NV_PATCH_SWATCH_SAVE_FIRST_ROW  0x00001000
#define NV_PATCH_SWATCH_SAVE_LAST_ROW   0x00002000
#define NV_PATCH_SWATCH_SAVE_FIRST_COL  0x00004000
#define NV_PATCH_SWATCH_SAVE_LAST_COL   0x00008000

#define NV_PATCH_HAS_NORMAL             0x00010000      // indicates to emit normal guard curves
#define NV_PATCH_NO_REGULAR_GRID        0x00020000      // indicates there is no regular grid -- only transitions

// ***** bits for info->flags
#define NV_PATCH_FLAG_TYPE_MASK         0x00000001
#define NV_PATCH_FLAG_TYPE_TRIANGULAR   0x00000001
#define NV_PATCH_FLAG_TYPE_PATCH        0x00000000

#define NV_PATCH_FLAG_TESS_MASK         0x00000002
#define NV_PATCH_FLAG_TESS_FRACTIONAL   0x00000002
#define NV_PATCH_FLAG_TESS_INTEGER      0x00000000

#define NV_PATCH_FLAG_AUTO_MASK         0x0000000C
#define NV_PATCH_FLAG_AUTO_NORMAL       0x00000004
#define NV_PATCH_FLAG_AUTO_UV           0x00000008
#define NV_PATCH_FLAG_AUTO_NONE         0x00000000

#define NV_PATCH_FLAG_CACHE_MASK        0x00000010
#define NV_PATCH_FLAG_DO_NOT_CACHE      0x00000010
#define NV_PATCH_FLAG_SOME_DIRTY_TESS   0x00000100

#define NV_PATCH_FLAG_TRANSPOSE         0x00001000
#define NV_PATCH_FLAG_FLIPPED_U         0x00002000
#define NV_PATCH_FLAG_FLIPPED_V         0x00004000

// bits for transition flag 
#define FIRST_ROW 0x1   //first row transition
#define LAST_ROW  0x2   //last row transition
#define FIRST_COL 0x4   //first column transition
#define LAST_COL  0x8   //last column transition
#define USE_ITE   0x10  //send an inner transition endpoint

// Information about the current patch
typedef struct {
    // Bitmask of which attributes to evaluate (bits 0..15)
    unsigned int evalEnables;   // as specified to the HW interface
    unsigned int swEnables;     // as presented by the SW interface
    // currently, all attributes handled in the shared code are processed
    // and passed to the back-end without any remapping.
    // For D3D, this is totally handeled in higher level code.
    // So, for D3D, all indices in the shared code are HW indices.
    // For OpenGL, when calling the immediate mode interface, the imm mode
    // calls remap from SW to HW indices.
    // For OpenGL, when calling the Kelvin HW, the remap occurs in
    // __glNVEvalMaps as info is copied from the 'gc' to 'info'

    int nAttr;              // number of active attrs
    int maxAttr;            // max attributes required for malloc's
    int firstAttr;          // first attribute found -- used in memcopies
    unsigned int maxOrder;  // max order of all enabled attributes
    int maxSwatch;          // set according to HW or SW swathing
    int nSwatchU, nSwatchV; // swatches in patch, including Begin/End stitch
    int fracSwatchU, fracSwatchV; // swatches not counting inner frac stitches

    int swatchFlags;        // control during swathing using NV_PATCH_SWATCH_*
    int usingHW;            // set if using Kelvin HW
    int cpuType;            // as provided by the RM
    unsigned long counter;  // temporary storage for the current semphore count

    // Evaluation flags
    unsigned int flags;     // NV_PATCH_FLAG_*
    int flipT;              // set if (flags & NV_PATCH_FLAG_FLAG_TRANSPOSE)
    int flipU;              // set if (flags & NV_PATCH_FLAG_FLIPPED_U)
    int flipV;              // set if (flags & NV_PATCH_FLAG_FLIPPED_V)
    int flipUV;             // for stitches
    int flipTUV;            // combine transpose and both flips

    //MOVE TO QUADINFO ---------------->
    unsigned char* cachedPB;        // cached push buffer
    unsigned long  cachedPBSize;    // cached push buffer size
    unsigned long  cachedPBCounter; // hit count
    //<-------------------------

    // CW/CCW reversal for FRAC layered on INT
    int reverse;

    // Description of tesselation factors
    union {
        struct { float nu0, nu1, nv0, nv1; } tensor;
        struct { float n1, n2, n3;         } tri;
    } tess, originaltess;

    // The basis for the patch
    NV_PATCH_BASIS_TYPE basis;

    // Source and destination of AUTO_NORMAL calculations
    int srcNormal, dstNormal;
    int rational;
    float startu, endu, startv, endv;
    // Destination of AUTO_UV calculations
    int srcUV[8], dstUV[8]; //can have up to 8 sets??

    // Information for each map
    NV_PATCH_MAP_INFO maps[NV_PATCH_NUMBER_OF_ATTRIBS];

    // What type of backend we are writing to or calling to
    NV_PATCH_BACKEND_TYPE backendType;

    // Function pointers for shared code to call into
    NV_PATCH_BACKEND *backend;
    NV_PATCH_DRIVER_CALLBACKS *callbacks;

    // Output buffer (pushbuffer, DP2 stream, ...)
    //MAY BE DEPRECATED?????
    unsigned char *buffer;
    unsigned int bufferLength; // in bytes
    int vertexSize;            // expected # of dwords to output per vertex

    // Opaque context pointer
    void *context;

    // for cache-able info
    NV_PATCH_ALLOC_CACHE *pCache[MAX_EV_CACHE];
    unsigned int retVal;    // for deeply nested alloc errors

    float *normalPatch;     // for NV_PATCH_FLAG_AUTO_NORMAL
    float *UVPatch;         // for NV_PATCH_FLAG_AUTO_UV

    // These point to the guard corners of the regular grid. They are in this
    // top-level structure to be used by all patch types.
    // Instead of fancy attribute arrays, they are simply enumerated for
    // the two attributes which require guard corners: vertex, normal
    // Think of drawing the regular grid from lower-left to upper-right.
    // These names match that orientation so the regular grid routine doesn't
    // need to worry about various orientation flipping and swatching cases.
    // Perhaps LL and LR are not needed for HW, but they're set up for now.
    // Generally these points are the corners of the regular grid,
    // except for LL and LR on the bottom row with a bottom stitch.
    // In this case, they correspond to the swatch corners, and must be stepped
    // up if you want the regular grid.
    //  UL=[0][1]   UR=[1][1]
    //  LL=[0][0]   LR=[0][1]
    float *pSwatchCorner[NV_PATCH_NUMBER_OF_ATTRIBS][2][2]; // [v][u]

    // These are available storage for the corners of the regular grid on
    // the outside of all swatches.
    // Whereas, pSwatchCornerVertex and pSwatchCornerNormal will be
    // correct per sub-patch, these need only be fully valid
    // after all swatches are done
    // Here, the indices are "absolute" because the stitch code was written
    // to draw stitches on the Left, Right, Bot, Top, not relative to the
    // regular grid.
    float gridCorner[NV_PATCH_NUMBER_OF_ATTRIBS][2][2][4]; // [v][u][xyzw]
    int setGridCorner;  // mask indicating which gridCorner to assign

    // 'fracCorner' is for the additional corners created by the 
    // top and right frac stitches. Only used by SW. For HW, HW does it.
    // fracCorner[i][0][0] is the same as gridCorner[i][0][0]
    float fracCorner[NV_PATCH_NUMBER_OF_ATTRIBS][2][2][4]; // [v][u][xyzw]

    // SCRATCH SPACE
    // alloced once with the structure to prevent LOTS of malloc/free's later.
    // IMPORTANT! USE THE MACROs BEGIN/END_USING_TEMP_MATRIX() to help
    // catch cases where nested routines use the same temp
    NV_PATCH_CURVE_INFO tempCurve;
    FDMatrix tempMatrix;
    FDMatrix tempMatrix_m0;    
    FDMatrix tempMatrix_m1;
    FDMatrix tempMatrix_m2;
    FDMatrix reparam;                        // used for Tri INT and FRAC
    FDMatrix *reduceTri[NV_PATCH_MAX_ORDER]; // see ReparameterizeTriPatch()

    #define BEGIN_USING_TEMP_MATRIX(MATRIX)     \
        nvAssert(!(MATRIX).rows);               \
        nvAssert(!(MATRIX).columns);            \

    #define END_USING_TEMP_MATRIX(MATRIX)       \
        nvAssert(((MATRIX).rows = 0, 1));       \
        nvAssert(((MATRIX).columns = 0, 1));    \

    int bytesGuardCurve;        // bytes in truncated NV_PATCH_CURVE_INFO
    int bytesGuardCurveAllAttr; // bytes for one curve set (one curve for each active attribute)

    NV_PATCH_QUAD_INFO *quadInfo;   // alloc'd by callers

    // temp storage allocated in DrawIntPatch() and used by lower routines
    void *pScratchBase; // ptr to group of all scratch stuff for free

    // temp FD matrix for reg grid, small steps in V and U
    FDMatrix *ppMatrixSetSS0[NV_PATCH_NUMBER_OF_ATTRIBS];

    // temp* curves are for boundary curves in regular grid
    NV_PATCH_CURVE_INFO *tempVBegin[NV_PATCH_NUMBER_OF_ATTRIBS];
    NV_PATCH_CURVE_INFO *tempVEnd[NV_PATCH_NUMBER_OF_ATTRIBS];

    // guard sets. Now only for FRAC. Later for more.
    FDCurveAttrSet guardSetUInnerFrac;  // for non-swathing
    FDCurveAttrSet guardSetVInnerFrac;  // for non-swathing
    FDCurveAttrSet *ppGuardSetUInnerFrac1[1];   // for non-swathing
    FDCurveAttrSet *ppGuardSetVInnerFrac1[1];   // for non-swathing
    FDCurveAttrSet **ppGuardSetUInnerFrac;
    FDCurveAttrSet **ppGuardSetVInnerFrac;
} NV_PATCH_INFO;

/*****************************************************************************/
// forward declarations
int nvAllocInfoScratch(NV_PATCH_INFO *info);
int nvAllocFDMatricesEtc(NV_PATCH_INFO *info);
unsigned int nvEvalPatchSetup(NV_PATCH_INFO *info);
unsigned int nvEvalPatch(NV_PATCH_INFO *info);
unsigned int nvEvalPatchCleanup(NV_PATCH_INFO *info);

unsigned int DrawKelvinIntPatchGrid(NV_PATCH_INFO *info, NV_PATCH_QUAD_INFO *quadInfo, FDMatrix **matrices);
void SendGuardCurve(unsigned long curvetype, NV_PATCH_INFO *info, NV_PATCH_QUAD_INFO *quadInfo, unsigned long flags);
void ComputeKelvinHOSControlWords(NV_PATCH_INFO *info, NV_PATCH_QUAD_INFO *quadInfo, long flags);
void CopyCurve(NV_PATCH_INFO *info, NV_PATCH_CURVE_INFO *dst, NV_PATCH_CURVE_INFO *src);

void OffsetFDMatrix(NV_PATCH_INFO *info, FDMatrix *m, int a, int b);
void OffsetCurve(NV_PATCH_INFO *info, NV_PATCH_CURVE_INFO *curve, int n);
void ExtractUCurve(NV_PATCH_INFO *info, NV_PATCH_CURVE_INFO *dst, const FDMatrix *m);
void ExtractVCurve(NV_PATCH_INFO *info, NV_PATCH_CURVE_INFO *dst, const FDMatrix *m);
void RetessellateFDCurve(NV_PATCH_INFO *info, float stepOld, float stepNew,
                                NV_PATCH_CURVE_INFO *curve, int reversedirection);
void RetessellateFDMatrix(NV_PATCH_INFO *info, float stepOldU, float stepOldV,
                             float stepNewU, float stepNewV, FDMatrix *matrix);

int evAllocInitWithCache(NV_PATCH_INFO *info, void *context, 
                            NV_PATCH_ALLOC_CONTROL *pevMemory,
                            int tBytes, NV_PATCH_ALLOC_CACHE *pCache);

extern const int attrHwToSw[NV_PATCH_NUMBER_OF_ATTRIBS];
extern const int attrSwToHw[NV_PATCH_NUMBER_OF_ATTRIBS];

//#define HOS_DEBUG
#if !defined(IS_OPENGL)
    //d3d defs
    #define VIEW_AS_DWORD(f) (*(DWORD *)(&(f)))
    #define HOS_KELVIN_CHANNEL                  NV_DD_KELVIN

    #define HOS_CREATE_AND_ASSIGN_PUSHBUF_LOCALS() \
            extern int fakeFunc(int ii)         // make declaration instead of <null> so macro may be called with ";"

    #ifdef HOS_DEBUG                            //for debug builds output debug info rather than actual pushbuffer methods
        #define HOS_PUSH(index, method)         { getDC()->nvPusher.push(index, method); \
                                                  DPF_LEVEL(NVDBG_LEVEL_HOS_INFO,"HOS: 0x%08x", (method)); } 
  
        #define HOS_PUSHF(index, method)        { getDC()->nvPusher.push(index, method); \
                                                  DPF_LEVEL(NVDBG_LEVEL_HOS_INFO,"HOS: %f", (*(float *)(&(method)))); }   
    
        #define HOS_PUSH_ADJUST(num_dwords)     getDC()->nvPusher.adjust(num_dwords)
        #define HOS_NOTE(a)                     DPF_LEVEL(NVDBG_LEVEL_HOS_INFO,"HOS: %s",a)                    
        #define HOS_FLUSH()                     //getDC()->nvPusher.flush(TRUE,CPushBuffer::FLUSH_HEAVY_POLLING)       
    #else
        #define HOS_PUSH(index, method)         getDC()->nvPusher.push(index, method)
        #define HOS_PUSHF(index, method)        HOS_PUSH(index,method)      //remap to same push method.
        #define HOS_PUSH_ADJUST(num_dwords)     getDC()->nvPusher.adjust(num_dwords)
        #define HOS_NOTE(a)                     //DPF_LEVEL(NVDBG_LEVEL_HOS_INFO,"HOS: %s",a)
        #define HOS_FLUSH()                     //getDC()->nvPusher.flush(TRUE,CPushBuffer::FLUSH_HEAVY_POLLING)
    #endif

#else   // !defined(IS_OPENGL)
    //OGL defs
    #define VIEW_AS_DWORD(f) (*(NvU32 *)(&(f)))
    #define HOS_KELVIN_CHANNEL NV_SC_3D         // define kelvin channel for OGL
    #define MAX_CHECK_PUSH_WORDS    100

    static NV_INLINE int dbgPushCheckCount(int nWritten, int *count)
    {
        int ii;

        for (ii = 0;  ii < nWritten;  ii++) {
            if (ii >= MAX_CHECK_PUSH_WORDS) {
                break;
            }
            assert(count[ii] == 1);
        }
        for (ii = 0;  ii < MAX_CHECK_PUSH_WORDS;  ii++) {
            count[ii] = 0;
        }
        return 0;
    }

    #define HOS_CREATE_AND_ASSIGN_PUSHBUF_LOCALS() \
            NV_TRACE_CODE( \
                int dbgPushCount[MAX_CHECK_PUSH_WORDS]; \
                int dbgPushFoo = nvDebugLevel >= 2 ? \
                                    dbgPushCheckCount(0, dbgPushCount) : 0; \
                const int setupForMT = MT_PENDING; \
            ) \
            __GLNVcontext *hwcx = NV_GET_HWCX(info->context); \
            __GLNVstate *state = NV_GET_HWSTATE(info->context) \
            /* NV_CREATE_AND_ASSIGN_PUSHBUF_LOCALS() */ \

    #define HOS_PUSH(index, data) \
            /* nvCurrent[index].u = data; */ \
            hwcx->u.pushBuf.current[index].u = data; \
            NV_TRACE_CODE( \
                assert((index) >= 0); \
                if ((index) < MAX_CHECK_PUSH_WORDS) { \
                    dbgPushCount[index]++; \
                } \
            ); \

    #define HOS_PUSHF(index, data) HOS_PUSH(index,data)

    #define HOS_PUSH_ADJUST(num_dwords) { \
            NV_CREATE_AND_ASSIGN_PUSHBUF_LOCALS(); \
            NV_TRACE_CODE( \
                if (nvDebugLevel >= 2) { \
                    dbgPushCheckCount(num_dwords, dbgPushCount); \
                } \
            ); \
            nvCurrent += (num_dwords); \
            NV_SYNC_PUSHBUF_LOCALS(); \
            NV_GET_VARIABLE_SPACE_IF_NEEDED(128); \
            nvAssert(nvCurrent < hwcx->u.pushBuf.fence); \
        }

    #define HOS_NOTE(a)

    #define HOS_FLUSH() { \
            NV_CREATE_AND_ASSIGN_PUSHBUF_LOCALS(); \
            NV_GET_SKID_SPACE_IF_NEEDED(); \
            NV_FLUSH(state); \
        }

    extern int enableUniqueCheck;
    extern int nUniqueVertices;
    void drawUniqueVertices(void);
#endif  // else !defined(IS_OPENGL)

#define HOS_PUSH4F(index, method)       { HOS_PUSHF(index+0, VIEW_AS_DWORD(method[0])); \
                                          HOS_PUSHF(index+1, VIEW_AS_DWORD(method[1])); \
                                          HOS_PUSHF(index+2, VIEW_AS_DWORD(method[2])); \
                                          HOS_PUSHF(index+3, VIEW_AS_DWORD(method[3])); }


// Assembly inner loops storage, constants, and defines
extern int bFirstTimeInit;           //global value to indicate if we've built dynamic inner loops yet or not.
typedef void (FASTCALL *PFOFSTMED)(NV_PATCH_CURVE_INFO *dst, int dir, FDMatrix *m, int a, int b);
typedef void (FASTCALL *PFOFSTM)(FDMatrix *m, int a, int b);
typedef void (FASTCALL *PFOFSC)(NV_PATCH_CURVE_INFO *curve, int n);
typedef void (FASTCALL *PFMM)(FDMatrix *dst, FDMatrix *a, FDMatrix *b);

#define MAX_PATCH_INNERLOOPS          16
#define OFFSET_FDMATRIX_ED_2x2      0
#define OFFSET_FDMATRIX_ED_4x4      1
#define OFFSET_FDMATRIX_ED_6x6      2
#define OFFSET_FDMATRIX_ED_DEF      3
#define OFFSET_FDMATRIX_2x2         4
#define OFFSET_FDMATRIX_4x4         5
#define OFFSET_FDMATRIX_6x6         6
#define OFFSET_FDMATRIX_DEF         7
#define OFFSET_CURVE_2x2            8
#define OFFSET_CURVE_4x4            9
#define OFFSET_CURVE_6x6            10
#define OFFSET_CURVE_DEF            11
#define OFFSET_MATRIX_MULT_2x2      12
#define OFFSET_MATRIX_MULT_4x4      13
#define OFFSET_MATRIX_MULT_6x6      14
#define OFFSET_MATRIX_MULT_DEF      15

typedef struct {
    //ptrs for inner loops
    void *HOSInnerLoops[MAX_PATCH_INNERLOOPS];  //function mappings

    //small FDMatrix cache to get rid of extra 
    unsigned int RetessFDtag[2][2];           //tags for a retessellation matrix cache
    FDMatrix *RetessFDCache[2];               //ptrs for retessellation matrix cache
    int RetessFDMRU;                          //MRU bit for the above cache
    
    //EV_ALLOC data structures to minimize the number of malloc/frees in shared code
    NV_PATCH_ALLOC_CACHE pCache[MAX_EV_CACHE];
    
    //callback ptrs for HOS routines
    NV_PATCH_DRIVER_CALLBACKS *callbacks;
} NV_PATCH_STATIC_DATA;


//forward declarations for functions used elsewhere
void OffsetFDMatrix_Extract_Discard(NV_PATCH_INFO *info, NV_PATCH_CURVE_INFO *dst, int dir, FDMatrix *m, int a, int b);
void nvPatchDestroyFD();
void nvPatchInitFD(NV_PATCH_INFO *info);

typedef struct{    
    //semaphore data members
    long *FDSynchCounter;
    unsigned char *baseAllocation;
    unsigned char *alignedAllocation;
    unsigned long FDContextDMA;    
    unsigned long cpuType;
    int bForceSoftware;                  //indicates we failed to set up semaphore -- force a SW fallback
    int bFirstTimeInit;    
    //inner loop data members
    void *HOSInnerLoops[MAX_PATCH_INNERLOOPS];  //function mappings
    //retessellation cache members
    unsigned int RetessFDtag[2][2];           //tags for a retessellation matrix cache
    FDMatrix *RetessFDCache[2];               //ptrs for retessellation matrix cache
    int RetessFDMRU;                          //MRU bit for the above cache

} NV_PATCH_GLOBAL_DATA;
//this should match the intiialization of the above default structure
#define DEFAULT_PATCH_GLOBAL_DATA {NULL,NULL,NULL,0,0,0,1, {NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL, \
                                                            NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL}, \
                                   {{0xFFFFFFFF,0xFFFFFFFF},{0xFFFFFFFF,0xFFFFFFFF}}, \
                                   {NULL,NULL}, 0} \

extern NV_PATCH_ALLOC_CACHE MyCache[MAX_EV_CACHE];
extern NV_PATCH_GLOBAL_DATA g_FDGlobalData;
//#define DCR_SEMAPHORE
//extern NV_PATCH_MUTEX_DATA g_FDMutexData;

#endif
