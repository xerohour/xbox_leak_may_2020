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

/*
** Standard polymorphic data types that are not present in older header files.
*/
#if defined(_WIN32) && (_MSC_VER < 1300)

typedef int            intptr_t;
typedef unsigned int   uintptr_t;

#endif

/*** Global constants ***/

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

// The maximum size of a swatch is determined by the size of the vertex
// file.   Henry says this is the magic number.  16 might work better, but
// it's not clear.
#define NV_PATCH_KELVIN_SWATCH_SIZE     17


#define NV_PATCH_MAX_TOTAL_STEP 1000    // total steps allowed
#define NV_PATCH_MAX_EVAL_STEP 40       // max steps within a sub-patch

// Matrix structure for matrix math
typedef struct {
    int rows, columns;
    float data[NV_PATCH_MAX_ORDER][NV_PATCH_MAX_ORDER][4]; // [row][col][xyzw]
} FDMatrix;

// For swathing, an array of ptrs to FDMatrices is used to 
// find a pointer to a particular FDMatrix given an attribute number
typedef FDMatrix *FDMatrixAttrSet[NV_PATCH_NUMBER_OF_ATTRIBS];

// Evaluated vertex attributes
typedef struct {
    float vertexAttribs[NV_PATCH_MAX_EVAL_STEP][NV_PATCH_NUMBER_OF_ATTRIBS][4];
} NV_PATCH_EVAL_OUTPUT;


// This is given to beginPrimitive to indicate what type of primitive to draw.
typedef enum {
    NV_PATCH_PRIMITIVE_TSTRIP = 1,
    NV_PATCH_PRIMITIVE_TFAN   = 2,
} NV_PATCH_PRIMITIVE_TYPE;

// Information for forward differencing one curve
typedef float NV_PATCH_CURVE_COEFFS[NV_PATCH_MAX_ORDER][4];
typedef struct {
    int order;
    //fffloat coeffs[NV_PATCH_MAX_ORDER][4];
    NV_PATCH_CURVE_COEFFS coeffs;
} NV_PATCH_CURVE_INFO;

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
    NV_PATCH_VERTEX_FORMAT_UBYTE   = 4, // Unpack bytes and remap to [0,1.0]    
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
    NV_PATCH_CURVE_INFO guardU00;           //       *------U01-----*------U11-----*
    NV_PATCH_CURVE_INFO guardU01;           //       |\_            |            _/|
    NV_PATCH_CURVE_INFO guardU10;           //       |  \_          |          _/  |
    NV_PATCH_CURVE_INFO guardU11;           //      V01  _\| 01    VC1    11 |/_  V11
    NV_PATCH_CURVE_INFO guardV00;           //       |              |              |
    NV_PATCH_CURVE_INFO guardV01;           //       |              |center        |
    NV_PATCH_CURVE_INFO guardV10;           //  v0Mid*------UC0-----*------UC1-----* v1Mid
    NV_PATCH_CURVE_INFO guardV11;           //       |              |              |
    NV_PATCH_CURVE_INFO guardUCenter0;      //       |    _         |         _    |
    NV_PATCH_CURVE_INFO guardUCenter1;      //      V00  _/|  00   VC0    10 |\_  V10
    NV_PATCH_CURVE_INFO guardVCenter0;      //       | _/           |           \_ |
    NV_PATCH_CURVE_INFO guardVCenter1;      //       |/             |             \|
    float u0Mid[4], v0Mid[4], u1Mid[4];     //       *------U00-----*------U10-----*
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


// Guard information for a single attribute for an integrally tesselated
// tensor product patch
typedef struct {                                                //          
    NV_PATCH_CURVE_INFO guardU0;                                //     c01                     c11 
    NV_PATCH_CURVE_INFO guardU1;                                //      *--------gU1------------*     
    NV_PATCH_CURVE_INFO guardV0;                                //      |                     _/|
    NV_PATCH_CURVE_INFO guardV1;                                //      |                   /   |               
    NV_PATCH_CURVE_INFO guardUInner;                            //      +------UInner-------+   |               
    NV_PATCH_CURVE_INFO guardVInner;                            //      |                   |   |               
    // *Begin/End point to guard curves at                      //      |                   |   |               
    //  begin/end of regular grid                               //      |                   |   |               
    NV_PATCH_CURVE_INFO *pGuardUBegin;                          //     gV0              VInner gV1               
    NV_PATCH_CURVE_INFO *pGuardUEnd; // row: U0, U1, UInner     //      |                   |   |               
    NV_PATCH_CURVE_INFO *pGuardVBegin;                          //      |                   |   |               
    NV_PATCH_CURVE_INFO *pGuardVEnd; // col: V0, V1, VInner     //      |                   |   |               
    // corners of the original control points                   //      |                   |   |               
    float corners[2][2][4]; // [v][u][xyzw]                     //      |                   |   |               
    // these cached here to use when dirtyFlags not set         //      +--------gU0--------+---*
    int u0Dir, u1Dir, v0Dir, v1Dir;                             //     c00                 pVB c10

    // *Begin/End point to guard curves at begin/end of regular grid

    // the corners[] are from the original control points
} NV_PATCH_INT_QUAD_GUARD_INFO;


// Information for an integrally tesselated tensor product patch
typedef struct {
    NV_PATCH_INT_QUAD_GUARD_INFO **guard;   // from guardQI or swathing
    int nu0, nv0, nu1, nv1; // actual number of segments along each edge
    int uMaxSegs, vMaxSegs, uMinSegs, vMinSegs; // MIN and MAX comparing nu0:nu1 and nv0:nv1

    // flags for transitions required.
    // Names refer to original LRTB, not flipped versions or anything
    int stitchLeft, stitchRight, stitchTop, stitchBottom;

    // these begin/end's mean in the direction of U (L/R) and V (B/T)
    // XXX FBF I find this confusing, since it doesn't correspond to U0, U1
    // XXX in other words, stitchVBegin means U0/UInner create a stitch (in cononical view)
    // XXX will soon change to make these match
    int stitchUBegin, stitchUEnd, stitchVBegin, stitchVEnd;

    int u0Dir, v0Dir, u1Dir, v1Dir; // '1' means normal direction (For u this is L-->R, for v this is B-->T)

    // MaxDir are directions of regular grid
    // XXX REMOVE? MinDir are opposite of MaxDir's. Doesn't appear to be used any more.
    int uMaxDir, vMaxDir, uMinDir, vMinDir;

    float du0, du1, dv0, dv1, duMax, dvMax; // small step size along edges, and for regular grid

    // temp storage allocated in DrawIntPatch() and used by lower routines
    // uGuard and vGuard are for the ending curves in regular grid
    NV_PATCH_CURVE_INFO **uGuard, **vGuard, *tempCurve;
    FDMatrix **ppMatrixSetSS0; // FD matrix for reg grid, small steps in V and U
    FDCurveAttrSet *pCurvesTop, *pCurvesBot;
    NV_PATCH_EVAL_OUTPUT *pOut1, *pOut2;
} NV_PATCH_INT_QUAD_INFO;

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
    int stride;                       // OGL: stride == type*sizeof(float)
    int pitch;                        // OGL: pitch == uorder
    NV_PATCH_VERTEX_FORMAT_TYPE type; 
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
} NV_PATCH_BACKEND;

// These are the callbacks that the shared code needs access to.  It is NOT SAFE
// to call C standard library or Windows API functions from the shared code!
typedef struct {
    void *(*malloc)(void *context, size_t bytes);
    void (*free)(void *context, void *ptr);    
    void (*memcpy)(void *dst, const void *src, size_t bytes);
    void (*flushOutput)(void *context, unsigned int bytesWritten);
} NV_PATCH_DRIVER_CALLBACKS;

/*****************************************************************************/
// eval memory allocation. Avoid seperate malloc for each section of memory.
// allocate a big chunk with EV_ALLOC_INIT 
// and dole it out efficiently with EV_ALLOC
// free it with EV_FREE
// Only one DECL per routine.
// Use EV_ALLOC_INIT to init locally.
// Use EV_ALLOC_RESET to use memory chunk allocated by higher level routine.

typedef struct {
        void *pBase;
        int callerAlloc;    // non-zero if caller does alloc/free
        void *pAvail;
        void *context;
        int bytesAlloc;
        int bytesUsed;
} NV_PATCH_ALLOC_CONTROL;

#define EV_ALLOC_DECL() NV_PATCH_ALLOC_CONTROL evMemory = {NULL, 0}

#define EV_ALLOC_RESET(EVMEMORY, CONTEXT, TOTALBYTES, errorLabel) do { \
        if (&(EVMEMORY) != &evMemory) { \
            evMemory = EVMEMORY; \
        } \
        nvAssert(CONTEXT == evMemory.context); \
        evMemory.pAvail = evMemory.pBase; \
        evMemory.bytesUsed = 0; \
        nvAssert((TOTALBYTES) <= evMemory.bytesAlloc); \
        if (!evMemory.pBase) { \
            retVal |= NV_PATCH_EVAL_ERR_OUT_OF_MEMORY; \
            evMemory.bytesAlloc = 0; \
        } \
        break; \
evAllocError: \
        retVal |= NV_PATCH_EVAL_ERR_OUT_OF_MEMORY; \
        goto errorLabel; \
    } while (0);\

        //ffPrintInt(evMemory.bytesAlloc); \

#define EV_ALLOC_INIT(CONTEXT, TOTALBYTES, errorLabel) { \
        nvAssert(!evMemory.pBase); \
        evMemory.bytesAlloc = (TOTALBYTES); /* assign first to eval expr */ \
        evMemory.pBase = info->callbacks->malloc(CONTEXT, evMemory.bytesAlloc);\
        evMemory.context = CONTEXT; \
        EV_ALLOC_RESET(evMemory, CONTEXT, evMemory.bytesAlloc, errorLabel); \
    }

#define EV_ALLOC(VAR, CONTEXT, BYTES) { \
        int __nBytes; \
        nvAssert(CONTEXT == evMemory.context); \
        evMemory.bytesUsed += (__nBytes = (BYTES)); \
        nvAssert(evMemory.bytesUsed <= evMemory.bytesAlloc); \
        if (evMemory.bytesUsed <= evMemory.bytesAlloc) { \
            *(void **)(&(VAR)) = evMemory.pAvail; \
            evMemory.pAvail = (char *)evMemory.pAvail + __nBytes; \
            nvAssert(!((uintptr_t)evMemory.pAvail & 0x3)); \
        } else { \
            VAR = NULL; \
        } \
        if (!VAR) goto evAllocError; \
    }

#define EV_FREE(VAR, CONTEXT) { \
        nvAssert(CONTEXT == evMemory.context); \
        if (evMemory.pBase && (VAR) && !evMemory.callerAlloc) { \
            info->callbacks->free(CONTEXT, evMemory.pBase); \
            evMemory.pBase = NULL; \
        } \
        VAR = NULL; \
    }

#define EV_ALLOC_FULL(CONTEXT) \
        /* expect equal if pre-calc was accurate */ \
        nvAssert(evMemory.bytesUsed == evMemory.bytesAlloc);

// The above alloc macro's could be redefined to use normal malloc/free,
// with appropriate macro's being null.
// The notion of a global evMemory is more specific to the eval code.
#define EV_ALLOC_GLOBAL(EVMEMORY, CONTEXT, TOTALBYTES, errorLabel) { \
        nvAssert(!EVMEMORY.pBase); \
        EVMEMORY.callerAlloc = 1; \
        EVMEMORY.bytesAlloc = (TOTALBYTES); /* assign first to eval expr */ \
        EVMEMORY.pBase = info->callbacks->malloc(CONTEXT, TOTALBYTES); \
        EVMEMORY.context = CONTEXT; \
        if (EVMEMORY.pBase == NULL) { \
            goto errorLabel; \
        } \
    } 

#define EV_FREE_GLOBAL(EVMEMORY, CONTEXT) { \
        if (EVMEMORY.pBase) info->callbacks->free(CONTEXT, EVMEMORY.pBase); \
        EVMEMORY.pBase = NULL; \
    }


// ***** bits for info->swatchFlags
#define NV_PATCH_SWATCH                 0x00000001      // set when swathing

#define NV_PATCH_SWATCH_FIRST_ROW       0x00000100
#define NV_PATCH_SWATCH_LAST_ROW        0x00000200
#define NV_PATCH_SWATCH_FIRST_COL       0x00000400
#define NV_PATCH_SWATCH_LAST_COL        0x00000800

#define NV_PATCH_SWATCH_SAVE_FIRST_ROW  0x00001000
#define NV_PATCH_SWATCH_SAVE_LAST_ROW   0x00002000
#define NV_PATCH_SWATCH_SAVE_FIRST_COL  0x00004000
#define NV_PATCH_SWATCH_SAVE_LAST_COL   0x00008000

#define NV_PATCH_HAS_NORMAL             0x00010000      // indicates to emit normal guard curves
//#define NV_PATCH_NEW_SWATH              0x00020000    // indicates this is the first swatch in a swath
//#define NV_PATCH_BEGIN_PATCH            0x00040000    // indicates this is the first swatch in a patch     
//#define NV_PATCH_END_PATCH              0x00080000    // indicates this is the last swatch in a patch


/*****************************************************************************/
// Flags for info->flags 
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

// Information about the current patch
typedef struct {
    // Bitmask of which attributes to evaluate (bits 0..15)
    unsigned int evalEnables;
    int nAttr;                  // number of active attrs
    int maxAttr;                // max attributes required for malloc's
    unsigned int maxOrder;      // max order of all enabled attributes
    int maxSwatch;              // set according to HW or SW swathing
    int nSwatchU, nSwatchV;     // swatches in patch, including Begin/End stitch

    int swatchFlags;            // control during swathing using NV_PATCH_SWATCH_*

    // Evaluation flags
    unsigned int flags;         // NV_PATCH_FLAG_*

    // Description of tesselation factors
    union {
        struct {
            float nu0, nu1, nv0, nv1;
        } tensor;
        struct {
            float n1, n2, n3;
        } tri;
    } tess, originaltess;

    // The basis for the patch
    NV_PATCH_BASIS_TYPE basis;

    // Source and destination of AUTO_NORMAL calculations
    int srcNormal, dstNormal;
    int rational;
    float startu, endu, startv, endv;
    // Destination of AUTO_UV calculations
    int srcUV[8], dstUV[8];         //can have up to 8 sets??

    // Information for each map
    NV_PATCH_MAP_INFO maps[NV_PATCH_NUMBER_OF_ATTRIBS];

    // What type of backend we are writing to or calling to
    NV_PATCH_BACKEND_TYPE backendType;

    // Function pointers for shared code to call into
    NV_PATCH_BACKEND *backend;
    NV_PATCH_DRIVER_CALLBACKS *callbacks;

    // Output buffer (pushbuffer, DP2 stream, ...)
    unsigned char *buffer;
    unsigned int bufferLength; // in bytes
    int vertexSize;            // expected # of dwords to output per vertex

    // Opaque context pointer
    void *context;

    // info for EV_ALLOC in lower level routines
    // for ComputeFDMatrix and ComputeFDCurveCoeffs()
    NV_PATCH_ALLOC_CONTROL evAllocComputeFD;

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
    float *pSwatchCornerVertex[2][2]; // [v][u]
    float *pSwatchCornerNormal[2][2]; // [v][u]

    // These are available storage for the corners of the regular grid on
    // the outside of all swatches.  
    // Whereas, pSwatchCornerVertex and pSwatchCornerNormal will be 
    // correct per sub-patch, these need only be fully valid
    // after all swatches are done
    // Here, the indices are "absolute" because the stitch code was written
    // to draw stitches on the Left, Right, Bot, Top, not relative to the
    // regular grid.
    float gridCornerVertex[2][2][4]; // [v][u][xyzw]
    float gridCornerNormal[2][2][4]; // [v][u][xyzw]
    int setGridCorner;  // mask indicating which gridCorner to assign

    // CACHED DATA
    // the driver should allocate memory for these data members BEFORE 
    // calling into the shared code.  Shared code should not be responsible 
    // for the life time of these data members.
    FDMatrix *m00[NV_PATCH_NUMBER_OF_ATTRIBS], *m01[NV_PATCH_NUMBER_OF_ATTRIBS],
             *m10[NV_PATCH_NUMBER_OF_ATTRIBS], *m11[NV_PATCH_NUMBER_OF_ATTRIBS];    //[attribute]  (sub-patch index = 0 for bezier types)

    //we should probably union these....
    NV_PATCH_FRAC_QUAD_GUARD_INFO *guardQF[NV_PATCH_NUMBER_OF_ATTRIBS];             //[attribute]  (sub-patch index = 0 bezier types)
    NV_PATCH_FRAC_TRI_GUARD_INFO  *guardTF[NV_PATCH_NUMBER_OF_ATTRIBS];
    NV_PATCH_INT_QUAD_GUARD_INFO  *guardQI[NV_PATCH_NUMBER_OF_ATTRIBS];   

} NV_PATCH_INFO;

unsigned int nvEvalPatch(NV_PATCH_INFO *info);

#endif
