// **************************************************************************
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
// ********************************* Direct 3D ******************************
//
//  Module: nvKelvinState.h
//      Kelvin state header file
//
// **************************************************************************
//
//  History:
//      Craig Duttweiler        27Mar2000         NV20 development
//
// **************************************************************************

#ifndef _NVKELVINSTATE_H
#define _NVKELVINSTATE_H

#if (NVARCH >= 0x020)

// general HW capabilities used internally by driver ------------------------

#define KELVIN_NUM_COMBINERS            8           // virtual combiner stages
#define KELVIN_NUM_MATRICES             4           // blendable matrices
#define KELVIN_NUM_TEXTURES             4           // texture units
#define KELVIN_NUM_WINDOWS              8           // clip windows

#define KELVIN_ALIGNMENT_MIN            64

#define KELVIN_VIEWPORT_BOUND           0           // kelvin viewport runs from 0..4096
#define KELVIN_D3D2OGL                  0.5f        // add 0.5 so that D3D pixels line up with OGL pixels
#define KELVIN_HALFSUBPIXELBIAS         0.03125f    // round to nearest 1/16 pixel grid

#define KELVIN_BORDER                   (-KELVIN_VIEWPORT_BOUND + KELVIN_D3D2OGL + KELVIN_HALFSUBPIXELBIAS)

// z depths
#define KELVIN_Z_SCALE16                65535.0f     // 2^16 - 1
#define KELVIN_Z_SCALE24                16777215.0f  // 2^24 - 1
#define KELVIN_Z_SCALE16_INV            (1.0f / KELVIN_Z_SCALE16)
#define KELVIN_Z_SCALE24_INV            (1.0f / KELVIN_Z_SCALE24)

#define KELVIN_TETRIS_THRESHOLD         1024         // minimum width of a clear that will get striped
#define KELVIN_TETRIS_WIDTH             256          // width of clear stripe in pixels (optimal = 1024 bytes)
                                                     // This needs to be adjusted by the color depth, as it's only accurate for 32bpp.
                                                     // It turns out that for NV20, it's much faster to clear 1024 byte swaths of
                                                     // the Z buffer.


#define KELVIN_MAX_INLINE_STRIDE        16    // largest number of DWORDs allowed in a single vertex
#define KELVIN_MAX_INLINE_VERTICES      120   // maximum vertices = maximum method data size / maximum vertex size
                                              //                  = 2^11 DWORDs / KELVIN_MAX_INLINE_STRIDE
                                              //                  = 128
#define KELVIN_MAX_NONINC_DATA          1000  // maximum amount of data (DWORDs) that can be sent to a non-incrementing
                                              // method like NV097_ARRAY_ELEMENT16 (actually 2^10-1 = 1023)

// general HW capabilities exported to D3D ----------------------------------

#define KELVIN_MAXVERTEXCOUNT                       32768
#define KELVIN_MAXBUFFERSIZE                        0

#define KELVIN_CAPS_GUARD_BAND_LEFT                -1.0e8f   // nv10/20 can use a guardband as large as 1e25, but 1e8 appears
#define KELVIN_CAPS_GUARD_BAND_TOP                 -1.0e8f   // to be the largest value that the game re-volt will handle.
#define KELVIN_CAPS_GUARD_BAND_RIGHT                1.0e8f   // anything larger aggravates an app bug that causes it to
#define KELVIN_CAPS_GUARD_BAND_BOTTOM               1.0e8f   // send us completely broken geometry.

#define KELVIN_CAPS_MAX_TEXTURE_REPEAT              8192 // the constraint on nv10 is actually du/dx < 8192
#define KELVIN_CAPS_MAX_TEXTURE_ASPECT_RATIO        0
#define KELVIN_CAPS_MAX_ANISOTROPY                  8    // Means we support anisotropic filtering.
#define KELVIN_CAPS_EXTENTS_ADJUST                  0

#define KELVIN_CAPS_MAX_UV_PAIRS                    8    // Number of texture uv's supported.

#define KELVIN_CAPS_FVF_CAPS                        (KELVIN_CAPS_MAX_UV_PAIRS | D3DFVFCAPS_DONOTSTRIPELEMENTS)
#define KELVIN_CAPS_MAX_TEXTURE_BLEND_STATES        KELVIN_NUM_COMBINERS
#define KELVIN_CAPS_MAX_SIMULTANEOUS_TEXTURES       KELVIN_NUM_TEXTURES

// DX7 stuff
#define KELVIN_CAPS_MAX_ACTIVE_LIGHTS               8
#define KELVIN_CAPS_MAX_USER_CLIP_PLANES_EXPORTED   0    // we can do 16, but it's not orthogonal to texturing, so WHQL won't let us export any
#define KELVIN_CAPS_MAX_USER_CLIP_PLANES_ALLOWED    8    // we can do 16, but nv30 will only do 8, so only process that many and ignore others
#define KELVIN_CAPS_MAX_VERTEX_BLEND_MATRICES       KELVIN_NUM_MATRICES

// DX8 stuff
#define KELVIN_CAPS_MAX_STREAMS                     16
#define KELVIN_CAPS_MAX_PSHADER_CONSTS              16
#define KELVIN_CAPS_MAX_VSHADER_CONSTS              96
#define KELVIN_CAPS_MAX_VSHADER_TEMPS               16

// maximum fog scale = 2^13. (the HW has 14 bits. we leave one for roundoff)
#define KELVIN_MAX_FOG_SCALE                        8192.0f

//---------------------------------------------------------------------------

// combiner inputs
typedef enum _kelvinCombinerInput {
    kelvinCombinerInputA = 0,
    kelvinCombinerInputB = 1,
    kelvinCombinerInputC = 2,
    kelvinCombinerInputD = 3,
} kelvinCombinerInput;

// the positions of the inputs in the input control words
static DWORD kelvinCombinerInputShift[4] = {24, 16, 8, 0};

//---------------------------------------------------------------------------
// dirty bits

// celsius uses 0x0000FFFF
// kelvin  uses 0xFFFF0000
#define KELVIN_DIRTY_COMBINERS_COLOR   0x00010000
#define KELVIN_DIRTY_COMBINERS_SPECFOG 0x00020000
#define KELVIN_DIRTY_CONTROL0          0x00040000
#define KELVIN_DIRTY_DEFAULTS          0x00080000
#define KELVIN_DIRTY_FVF               0x00100000
#define KELVIN_DIRTY_LIGHTS            0x00200000
#define KELVIN_DIRTY_MISC_STATE        0x00400000
#define KELVIN_DIRTY_SURFACE           0x00800000
#define KELVIN_DIRTY_TEXTURE_STATE     0x01000000
#define KELVIN_DIRTY_TEXTURE_TRANSFORM 0x02000000
#define KELVIN_DIRTY_TRANSFORM         0x04000000
#define KELVIN_DIRTY_VERTEX_SHADER     0x08000000
#define KELVIN_DIRTY_PIXEL_SHADER      0x10000000

#define KELVIN_DIRTY_REALLY_FILTHY     (KELVIN_DIRTY_COMBINERS_COLOR   | \
                                        KELVIN_DIRTY_COMBINERS_SPECFOG | \
                                        KELVIN_DIRTY_CONTROL0          | \
                                        KELVIN_DIRTY_DEFAULTS          | \
                                        KELVIN_DIRTY_FVF               | \
                                        KELVIN_DIRTY_LIGHTS            | \
                                        KELVIN_DIRTY_MISC_STATE        | \
                                        KELVIN_DIRTY_SURFACE           | \
                                        KELVIN_DIRTY_TEXTURE_STATE     | \
                                        KELVIN_DIRTY_TEXTURE_TRANSFORM | \
                                        KELVIN_DIRTY_TRANSFORM         | \
                                        KELVIN_DIRTY_VERTEX_SHADER     | \
                                        KELVIN_DIRTY_PIXEL_SHADER)

//---------------------------------------------------------------------------

// values for the texture stage <=> texture unit mappings
#define KELVIN_UNUSED                          0xdeadbeef
#define KELVIN_COMBINER_REG_TEX0               0x00000008      // the combiner's first texture register (data from texture unit 0)

// values for texture unit status
#define KELVIN_TEXUNITSTATUS_IDLE              0x00000000      // texture unit is unused
#define KELVIN_TEXUNITSTATUS_USER              0x00000001      // texture unit has a typical user-supplied texture
#define KELVIN_TEXUNITSTATUS_BUMPMAPA          0x00000002      // texture unit has a user-supplied bumpmap in it (see below)
#define KELVIN_TEXUNITSTATUS_BUMPMAPB          0x00000004      //                          "
#define KELVIN_TEXUNITSTATUS_BUMPMAPC          0x00000008      //                          "
#define KELVIN_TEXUNITSTATUS_BUMPMAPD          0x00000010      //                          "
#define KELVIN_TEXUNITSTATUS_CLIP              0x00000020      // texture unit is being used to handle user clip plnes

// bumpmap flavors:
//   A: blend mode does not request luminance from texture, and texure does not have it
//   B: blend mode does not request luminance from texture, but texure does have it
//   C: blend mode requests luminance from texture, but texure does not have it
//   D: blend mode requests luminance from texture, and texure has it

// general kelvin state flags
#define KELVIN_FLAG_PASSTHROUGHMODE            0x00000001      // are we in passthrough mode?
#define KELVIN_FLAG_TEXSTATENEEDSINVMV         0x00000004      // inverse modelview matrix is needed b/c of texture state
#define KELVIN_FLAG_LIGHTINGNEEDSINVMV         0x00000008      // inverse modelview matrix is needed b/c of light state
#define KELVIN_FLAG_COMBINERSNEEDSPECULAR      0x00000010      // specular is referenced as a combiner (TSS) input
#define KELVIN_FLAG_SETUPFAILURE               0x00008000      // did we fail to give the app what it really wanted for one reason or another?

// kelvin texture unit and combiner state flags
#define KELVIN_FLAG_USERTEXCOORDSNEEDED(i)     (0x00010000 << (4*(i)))  // are user texture coordinates needed by HW texture unit i?
#define KELVIN_FLAG_USERTEXCOORDSNEEDED_MASK   (0x11110000)

#ifdef STOMP_TEX_COORDS
#define KELVIN_FLAG_STOMP_4TH_COORD(i)         (0x00020000 << (4*(i)))  // stupid app fix: stomp 4th texcoord to 1.0 when they moronically send down extraneous coordinates
#define KELVIN_FLAG_STOMP_4TH_COORD_MASK       (0x22220000)
#endif  // STOMP_TEX_COORDS

#define KELVIN_SET_NUM_TEX_COORDS(stage,num)   ((num) << (4*(stage)))
#define KELVIN_GET_NUM_TEX_COORDS(value,stage) (((value) >> (4*(stage))) & 0xf)

#ifdef STOMP_TEX_COORDS
#define KELVIN_MASK_INNERLOOPUNIQUENESS        (KELVIN_FLAG_USERTEXCOORDSNEEDED_MASK  |  \
                                                KELVIN_FLAG_STOMP_4TH_COORD_MASK)
#else  // !STOMP_TEX_COORDS
#define KELVIN_MASK_INNERLOOPUNIQUENESS        (KELVIN_FLAG_USERTEXCOORDSNEEDED_MASK)
#endif  // !STOMP_TEX_COORDS

// inner loop constants
#define KELVIN_ILMASK_PRIMTYPE        0x0000ffff
#define KELVIN_ILFLAG_LEGACY          0x00010000  // use legacy strides
//#define KELVIN_ILFLAG_CMHACK          0x00020000  // use tex matrix hack vertex copy routine
#define KELVIN_ILFLAG_NOCULL          0x00040000  // no culling

#define KELVIN_ILMASK_LOOPTYPE        0xf0000000  // type of inner loop
#define KELVIN_ILFLAG_IX_VB_PRIM      0x00000000
#define KELVIN_ILFLAG_IX_STR_PRIM     0x10000000
#define KELVIN_ILFLAG_IX_INL_PRIM     0x20000000
#define KELVIN_ILFLAG_IX_INL_TRI_LIST 0x40000000
#define KELVIN_ILFLAG_IX_DVB_TRI      0x60000000
#define KELVIN_ILFLAG_OR_VB_PRIM      0x80000000
#define KELVIN_ILFLAG_OR_STR_PRIM     0x90000000
#define KELVIN_ILFLAG_OR_INL_PRIM     0xa0000000
#define KELVIN_ILFLAG_OR_INL_TRI_LIST 0xc0000000
#define KELVIN_ILFLAG_SUPER_TRI_LIST  0xe0000000

#define KELVIN_SHORT_INDEXED_PRIM_COUNT_CUTOFF     25    // prim length before we introduce culling
#define KELVIN_SHORT_ORDERED_PRIM_COUNT_CUTOFF     25

// xPipe / Super-tri constants
#define KELVIN_CULLFLAG_MODELOK         0x00000001

// xPipe / Super-tri inner loop flags. These ought to be in the inner loop flags above, but we don't have enough
// flag bits.
#define KELVIN_STILFLAG_STDEFVB         0x00000001  // default VB is used (affects supertri logic)
#define KELVIN_STILFLAG_MODEL           0x00000002  // Model Culling
#define KELVIN_STILFLAG_XFORM           0x00000004  // Xform / clipcheck / cull
#define KELVIN_STILFLAG_HIBW            0x00000008  // High bandwidth mode -- check all triangles

typedef void (__stdcall *KELVINDISPATCHPRIMITIVE)(void);

//---------------------------------------------------------------------------

// forward declarations
class CVertexShader;

//---------------------------------------------------------------------------

class CKelvinState
{
    // intermediate state
public:

    // methods
public:

#if STATE_OPTIMIZE
    // push integer data from an array
    inline void seta (DWORD dwOffset, DWORD count, DWORD *dwArray)
    {
        if (!count) return;

        getDC()->nvPusher.push (0, (count << 18) | (NV_DD_KELVIN << 13) | dwOffset);
        for (DWORD i = 1; i <= count; i++)
        {
            getDC()->nvPusher.push(i, *dwArray++);
        }
        getDC()->nvPusher.adjust(count+1);
    }
#endif

    // integer data
    inline void set1 (DWORD dwOffset, DWORD dw0)
    {
        getDC()->nvPusher.push (0, 0x00040000 | (NV_DD_KELVIN << 13) | dwOffset);
        getDC()->nvPusher.push (1, dw0);
        getDC()->nvPusher.adjust (2);
    }
    inline void set2 (DWORD dwOffset, DWORD dw0, DWORD dw1)
    {
        getDC()->nvPusher.push (0, 0x00080000 | (NV_DD_KELVIN << 13) | dwOffset);
        getDC()->nvPusher.push (1, dw0);
        getDC()->nvPusher.push (2, dw1);
        getDC()->nvPusher.adjust (3);
    }
    inline void set3 (DWORD dwOffset, DWORD dw0, DWORD dw1, DWORD dw2)
    {
        getDC()->nvPusher.push (0, 0x000c0000 | (NV_DD_KELVIN << 13) | dwOffset);
        getDC()->nvPusher.push (1, dw0);
        getDC()->nvPusher.push (2, dw1);
        getDC()->nvPusher.push (3, dw2);
        getDC()->nvPusher.adjust (4);
    }
    inline void set4 (DWORD dwOffset, DWORD dw0, DWORD dw1, DWORD dw2, DWORD dw3)
    {
        getDC()->nvPusher.push (0, 0x00100000 | (NV_DD_KELVIN << 13) | dwOffset);
        getDC()->nvPusher.push (1, dw0);
        getDC()->nvPusher.push (2, dw1);
        getDC()->nvPusher.push (3, dw2);
        getDC()->nvPusher.push (4, dw3);
        getDC()->nvPusher.adjust (5);
    }
    inline void set5 (DWORD dwOffset, DWORD dw0, DWORD dw1, DWORD dw2, DWORD dw3, DWORD dw4)
    {
        getDC()->nvPusher.push (0, 0x00140000 | (NV_DD_KELVIN << 13) | dwOffset);
        getDC()->nvPusher.push (1, dw0);
        getDC()->nvPusher.push (2, dw1);
        getDC()->nvPusher.push (3, dw2);
        getDC()->nvPusher.push (4, dw3);
        getDC()->nvPusher.push (5, dw4);
        getDC()->nvPusher.adjust (6);
    }
    inline void set6 (DWORD dwOffset, DWORD dw0, DWORD dw1, DWORD dw2, DWORD dw3, DWORD dw4, DWORD dw5)
    {
        getDC()->nvPusher.push (0, 0x00180000 | (NV_DD_KELVIN << 13) | dwOffset);
        getDC()->nvPusher.push (1, dw0);
        getDC()->nvPusher.push (2, dw1);
        getDC()->nvPusher.push (3, dw2);
        getDC()->nvPusher.push (4, dw3);
        getDC()->nvPusher.push (5, dw4);
        getDC()->nvPusher.push (6, dw5);
        getDC()->nvPusher.adjust (7);
    }
    inline void set7 (DWORD dwOffset, DWORD dw0, DWORD dw1, DWORD dw2, DWORD dw3, DWORD dw4, DWORD dw5, DWORD dw6)
    {
        getDC()->nvPusher.push (0, 0x001c0000 | (NV_DD_KELVIN << 13) | dwOffset);
        getDC()->nvPusher.push (1, dw0);
        getDC()->nvPusher.push (2, dw1);
        getDC()->nvPusher.push (3, dw2);
        getDC()->nvPusher.push (4, dw3);
        getDC()->nvPusher.push (5, dw4);
        getDC()->nvPusher.push (6, dw5);
        getDC()->nvPusher.push (7, dw6);
        getDC()->nvPusher.adjust (8);
    }
    inline void set8 (DWORD dwOffset, DWORD dw0, DWORD dw1, DWORD dw2, DWORD dw3, DWORD dw4, DWORD dw5, DWORD dw6, DWORD dw7)
    {
        getDC()->nvPusher.push (0, 0x00200000 | (NV_DD_KELVIN << 13) | dwOffset);
        getDC()->nvPusher.push (1, dw0);
        getDC()->nvPusher.push (2, dw1);
        getDC()->nvPusher.push (3, dw2);
        getDC()->nvPusher.push (4, dw3);
        getDC()->nvPusher.push (5, dw4);
        getDC()->nvPusher.push (6, dw5);
        getDC()->nvPusher.push (7, dw6);
        getDC()->nvPusher.push (8, dw7);
        getDC()->nvPusher.adjust (9);
    }

    // floating-point data
    inline void set1f (DWORD dwOffset, float fData0)
    {
        getDC()->nvPusher.push (0, 0x00040000 | (NV_DD_KELVIN << 13) | dwOffset);
        getDC()->nvPusher.push (1, *(DWORD*)&fData0);
        getDC()->nvPusher.adjust (2);
    }
    inline void set2f (DWORD dwOffset, float fData0, float fData1)
    {
        getDC()->nvPusher.push (0, 0x00080000 | (NV_DD_KELVIN << 13) | dwOffset);
        getDC()->nvPusher.push (1, *(DWORD*)&fData0);
        getDC()->nvPusher.push (2, *(DWORD*)&fData1);
        getDC()->nvPusher.adjust (3);
    }
    inline void set3f (DWORD dwOffset, float fData0, float fData1, float fData2)
    {
        getDC()->nvPusher.push (0, 0x000c0000 | (NV_DD_KELVIN << 13) | dwOffset);
        getDC()->nvPusher.push (1, *(DWORD*)&fData0);
        getDC()->nvPusher.push (2, *(DWORD*)&fData1);
        getDC()->nvPusher.push (3, *(DWORD*)&fData2);
        getDC()->nvPusher.adjust (4);
    }
    inline void set4f (DWORD dwOffset,
                       float fData0, float fData1,
                       float fData2, float fData3)
    {
        getDC()->nvPusher.push (0, 0x00100000 | (NV_DD_KELVIN << 13) | dwOffset);
        getDC()->nvPusher.push (1, *(DWORD*)&fData0);
        getDC()->nvPusher.push (2, *(DWORD*)&fData1);
        getDC()->nvPusher.push (3, *(DWORD*)&fData2);
        getDC()->nvPusher.push (4, *(DWORD*)&fData3);
        getDC()->nvPusher.adjust (5);
    }
    inline void set8f (DWORD dwOffset,
                       float fData0, float fData1, float fData2, float fData3,
                       float fData4, float fData5, float fData6, float fData7)
    {
        getDC()->nvPusher.push (0, 0x00200000 | (NV_DD_KELVIN << 13) | dwOffset);
        getDC()->nvPusher.push (1, *(DWORD*)&fData0);
        getDC()->nvPusher.push (2, *(DWORD*)&fData1);
        getDC()->nvPusher.push (3, *(DWORD*)&fData2);
        getDC()->nvPusher.push (4, *(DWORD*)&fData3);
        getDC()->nvPusher.push (5, *(DWORD*)&fData4);
        getDC()->nvPusher.push (6, *(DWORD*)&fData5);
        getDC()->nvPusher.push (7, *(DWORD*)&fData6);
        getDC()->nvPusher.push (8, *(DWORD*)&fData7);
        getDC()->nvPusher.adjust (9);
    }

    // various routines to set texture transform matrices. see comments in nvSetKelvinTextureTransform()
    // (s,t,1.0) in, (s,t,1.0) out
    void setTextureTransformMatrixType220 (DWORD dwName, D3DMATRIX *pMatrix)
    {
        set4f (dwName +  0, pMatrix->_11, pMatrix->_21, 0.0f,         pMatrix->_31);
        set4f (dwName + 16, pMatrix->_12, pMatrix->_22, 0.0f,         pMatrix->_32);
        set4f (dwName + 32, 0.0f,         0.0f,         0.0f,         0.0f        );
        set4f (dwName + 48, 0.0f,         0.0f,         0.0f,         1.0f        );
    }
    // (s,t,1.0) in, (s,t,r,1.0) out
    void setTextureTransformMatrixType230 (DWORD dwName, D3DMATRIX *pMatrix)
    {
        set4f (dwName +  0, pMatrix->_11, pMatrix->_21, 0.0f,         pMatrix->_31);
        set4f (dwName + 16, pMatrix->_12, pMatrix->_22, 0.0f,         pMatrix->_32);
        set4f (dwName + 32, pMatrix->_13, pMatrix->_23, 0.0f,         pMatrix->_33);
        set4f (dwName + 48, 0.0f,         0.0f,         0.0f,         1.0f        );
    }
    // (s,t,1.0) in, (s,t,q) out
    void setTextureTransformMatrixType231 (DWORD dwName, D3DMATRIX *pMatrix)
    {
        set4f (dwName +  0, pMatrix->_11, pMatrix->_21, 0.0f,         pMatrix->_31);
        set4f (dwName + 16, pMatrix->_12, pMatrix->_22, 0.0f,         pMatrix->_32);
        set4f (dwName + 32, 0.0f,         0.0f,         0.0f,         0.0f        );
        set4f (dwName + 48, pMatrix->_13, pMatrix->_23, 0.0f,         pMatrix->_33);
    }
    // (s,t,1.0) in, (s,t,r,q) out
    void setTextureTransformMatrixType241 (DWORD dwName, D3DMATRIX *pMatrix)
    {
        set4f (dwName +  0, pMatrix->_11, pMatrix->_21, 0.0f,         pMatrix->_31);
        set4f (dwName + 16, pMatrix->_12, pMatrix->_22, 0.0f,         pMatrix->_32);
        set4f (dwName + 32, pMatrix->_13, pMatrix->_23, 0.0f,         pMatrix->_33);
        set4f (dwName + 48, pMatrix->_14, pMatrix->_24, 0.0f,         pMatrix->_34);
    }
    // (s,t,r,1.0) in, (s,t,1.0) out
    void setTextureTransformMatrixType320 (DWORD dwName, D3DMATRIX *pMatrix)
    {
        set4f (dwName +  0, pMatrix->_11, pMatrix->_21, pMatrix->_31, pMatrix->_41);
        set4f (dwName + 16, pMatrix->_12, pMatrix->_22, pMatrix->_32, pMatrix->_42);
        set4f (dwName + 32, 0.0f,         0.0f,         0.0f,         0.0f        );
        set4f (dwName + 48, 0.0f,         0.0f,         0.0f,         1.0f        );
    }
    // (s,t,r,1.0) in, (s,t,r,1.0) out
    void setTextureTransformMatrixType330 (DWORD dwName, D3DMATRIX *pMatrix)
    {
        set4f (dwName +  0, pMatrix->_11, pMatrix->_21, pMatrix->_31, pMatrix->_41);
        set4f (dwName + 16, pMatrix->_12, pMatrix->_22, pMatrix->_32, pMatrix->_42);
        set4f (dwName + 32, pMatrix->_13, pMatrix->_23, pMatrix->_33, pMatrix->_43);
        set4f (dwName + 48, 0.0f,         0.0f,         0.0f,         1.0f        );
    }
    // (s,t,r,1.0) in, (s,t,q) out
    void setTextureTransformMatrixType331 (DWORD dwName, D3DMATRIX *pMatrix)
    {
        set4f (dwName +  0, pMatrix->_11, pMatrix->_21, pMatrix->_31, pMatrix->_41);
        set4f (dwName + 16, pMatrix->_12, pMatrix->_22, pMatrix->_32, pMatrix->_42);
        set4f (dwName + 32, 0.0f,         0.0f,         0.0f,         0.0f        );
        set4f (dwName + 48, pMatrix->_13, pMatrix->_23, pMatrix->_33, pMatrix->_43);
    }
    // (s,t,r,1.0) in, (s,t,r,q) out
    void setTextureTransformMatrixType341 (DWORD dwName, D3DMATRIX *pMatrix)
    {
        set4f (dwName +  0, pMatrix->_11, pMatrix->_21, pMatrix->_31, pMatrix->_41);
        set4f (dwName + 16, pMatrix->_12, pMatrix->_22, pMatrix->_32, pMatrix->_42);
        set4f (dwName + 32, pMatrix->_13, pMatrix->_23, pMatrix->_33, pMatrix->_43);
        set4f (dwName + 48, pMatrix->_14, pMatrix->_24, pMatrix->_34, pMatrix->_44);
    }

    // set only the first three rows, since the rest is unnecessary
    void setInverseModelViewMatrix (DWORD dwName, D3DMATRIX *pMatrix)
    {
        set4f (dwName +  0, pMatrix->_11, pMatrix->_12, pMatrix->_13, pMatrix->_14);
        set4f (dwName + 16, pMatrix->_21, pMatrix->_22, pMatrix->_23, pMatrix->_24);
        set4f (dwName + 32, pMatrix->_31, pMatrix->_32, pMatrix->_33, pMatrix->_34);
    }

    // transpose the matrix since d3d does {v}[M] instead of [M]{v}'
    void setMatrixTransposed (DWORD dwName,D3DMATRIX *pMatrix)
    {
        set4f (dwName +  0, pMatrix->_11, pMatrix->_21, pMatrix->_31, pMatrix->_41);
        set4f (dwName + 16, pMatrix->_12, pMatrix->_22, pMatrix->_32, pMatrix->_42);
        set4f (dwName + 32, pMatrix->_13, pMatrix->_23, pMatrix->_33, pMatrix->_43);
        set4f (dwName + 48, pMatrix->_14, pMatrix->_24, pMatrix->_34, pMatrix->_44);
    }
};

// prototypes ---------------------------------------------------------------

HRESULT __stdcall       nvSetKelvinState           (PNVD3DCONTEXT pContext);
HRESULT __stdcall       nvSetKelvinClearState      (PNVD3DCONTEXT pContext);
KELVINDISPATCHPRIMITIVE nvKelvinGetDispatchRoutine (PNVD3DCONTEXT pConext,DWORD dwFlags);

#endif  // NVARCH >= 0x020

#endif  // _NVKELVINSTATE_H

