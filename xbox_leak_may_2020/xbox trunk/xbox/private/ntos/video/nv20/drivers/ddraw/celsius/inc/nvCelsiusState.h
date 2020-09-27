#ifndef _NVCELSIUSSTATE_H
#define _NVCELSIUSSTATE_H

/*
 * Copyright 1993-1998 NVIDIA, Corporation.  All rights reserved.
 *
 * THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
 * NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
 * IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: nvCelsiusState.h                                                  *
*     Constants and macros associated with nvCelsiusState.cpp               *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Craig Duttweiler    (bertrem)   22June99     created                *
*                                                                           *
\***************************************************************************/

#if (NVARCH >= 0x010)

// general HW capabilities exported to D3D ----------------------------------

#define CELSIUS_MAXVERTEXCOUNT                    32768
#define CELSIUS_MAXBUFFERSIZE                     0

#define CELSIUS_CAPS_GUARD_BAND_LEFT             -1.0e8f  // nv10 can use a guardband as large as 1e25, but 1e8 appears
#define CELSIUS_CAPS_GUARD_BAND_TOP              -1.0e8f  // to be the largest value that the game re-volt will handle.
#define CELSIUS_CAPS_GUARD_BAND_RIGHT             1.0e8f  // anything larger aggravates an app bug that causes it to
#define CELSIUS_CAPS_GUARD_BAND_BOTTOM            1.0e8f  // send us completely broken geometry.

#define CELSIUS_CAPS_MAX_TEXTURE_REPEAT           8192 // the constraint on nv10 is actually du/dx < 8192
#define CELSIUS_CAPS_MAX_TEXTURE_ASPECT_RATIO     0
#define CELSIUS_CAPS_MAX_ANISOTROPY               2    // Means we support anisotropic filtering.
#define CELSIUS_CAPS_EXTENTS_ADJUST               0

#define CELSIUS_CAPS_MAX_UV_PAIRS                 8    // Number of texture uv's supported.

#define CELSIUS_CAPS_FVF_CAPS                     (CELSIUS_CAPS_MAX_UV_PAIRS | D3DFVFCAPS_DONOTSTRIPELEMENTS)
#define CELSIUS_CAPS_MAX_TEXTURE_BLEND_STATES     8
#define CELSIUS_CAPS_MAX_SIMULTANEOUS_TEXTURES    2

// DX7 stuff
#define CELSIUS_CAPS_MAX_ACTIVE_LIGHTS            8
#define CELSIUS_CAPS_MAX_USER_CLIP_PLANES         0    // we can do 4, but it's not orthogonal to texturing, so WHQL won't let us export it
#define CELSIUS_CAPS_MAX_VERTEX_BLEND_MATRICES    2

// DX8 stuff
#define CELSIUS_CAPS_MAX_STREAMS                  16
#define CELSIUS_CAPS_MAX_PSHADER_CONSTS           16
#define CELSIUS_CAPS_MAX_VSHADER_CONSTS           0
#define CELSIUS_CAPS_MAX_VSHADER_TEMPS            0

// general HW capabilities used internally by driver ------------------------

#define CELSIUS_NUM_COMBINERS            2           // virtual combiner stages
#define CELSIUS_NUM_TEXTURES             2           // texture units

#define CELSIUS_ALIGNMENT_MIN           64
#define CELSIUS_PAGE_MASK               0xfffff000

#define CELSIUS_VIEWPORT_BOUND          2048

// Because our h/w is OpenGL'ized, we must D3D'ize it by adding 0.5 so
// that D3D pixels line up with OpenGL pixels.  Halfsubpixelbias is
// to adjust for floating point numbers which are not accurately
// represented.  E.g., -.7 is actually represented as -.699999
// If coordinates were only positive, this would be ok since it would
// result in being just to the left of the pixel center (integer coord).
// But, since the viewport is -2K...2K and the negative portion is used
// first, we actually end up being to the right of pixel center.
// Since the h/w doesn't do rounding to nearest 1/16 of a pixel, we
// adjust this here by subtracting 1/32 to round it.
#define CELSIUS_D3D2OGL                 0.5f
#define CELSIUS_HALFSUBPIXELBIAS        0.03125f     // round to nearest 1/16 pixel grid
#define CELSIUS_BORDER                  (-CELSIUS_VIEWPORT_BOUND + CELSIUS_D3D2OGL - CELSIUS_HALFSUBPIXELBIAS)

#define TEXTUREPM                       0xCAFEBEEF   // arbitrary identifier
#define TSS_USES_NULL_TEXTURE           0x1

#define CELSIUS_MAX_WINDOWS             8

#define CELSIUS_Z_SCALE16               65535.0f     // 2^16 - 1
#define CELSIUS_Z_SCALE24               16777215.0f  // 2^24 - 1
#define CELSIUS_Z_SCALE16_INV           (1.0f / CELSIUS_Z_SCALE16)
#define CELSIUS_Z_SCALE24_INV           (1.0f / CELSIUS_Z_SCALE24)

// maximum vertices = maximum method data size / maximum vertex size
//                  = 2^11 DWORDs / 16 DWORDs
//                  = 128
#define CELSIUS_MAX_INLINE_VERTICES     120

// maximum fog scale = 2^13. (the HW has 14 bits. we leave one for roundoff)
#define CELSIUS_MAX_FOG_SCALE           8192.0f

#define NONINC_METHOD(method) (0x40000000 | method)

// combiner input masks
#define CELSIUS_COMBINER_A_SOURCE_MASK  0x0f000000
#define CELSIUS_COMBINER_B_SOURCE_MASK  0x000f0000
#define CELSIUS_COMBINER_C_SOURCE_MASK  0x00000f00
#define CELSIUS_COMBINER_D_SOURCE_MASK  0x0000000f

// combiner inputs
typedef enum _celsiusCombinerInput {
    celsiusCombinerInputA = 0,
    celsiusCombinerInputB = 1,
    celsiusCombinerInputC = 2,
    celsiusCombinerInputD = 3,
} celsiusCombinerInput;

// the positions of the inputs in the input control words
static DWORD celsiusCombinerInputShift[4] = {24, 16, 8, 0};

//---------------------------------------------------------------------------
// dirty bits

// celsius uses 0x0000FFFF
// kelvin  uses 0xFFFF0000
#define CELSIUS_DIRTY_DEFAULTS          0x00000001
#define CELSIUS_DIRTY_TL_MODE           0x00000002
#define CELSIUS_DIRTY_FVF               0x00000004
#define CELSIUS_DIRTY_SURFACE           0x00000008
#define CELSIUS_DIRTY_TRANSFORM         0x00000010
#define CELSIUS_DIRTY_TEXTURE_TRANSFORM 0x00000020
#define CELSIUS_DIRTY_CONTROL0          0x00000040
#define CELSIUS_DIRTY_COMBINERS         0x00000080
#define CELSIUS_DIRTY_TEXTURE_STATE     0x00000100
#define CELSIUS_DIRTY_LIGHTS            0x00000200
#define CELSIUS_DIRTY_MISC_STATE        0x00000400
#define CELSIUS_DIRTY_SPECFOG_COMBINER  0x00000800
#define CELSIUS_DIRTY_PIXEL_SHADER      0x00001000

#define CELSIUS_DIRTY_REALLY_FILTHY     (CELSIUS_DIRTY_DEFAULTS          | \
                                         CELSIUS_DIRTY_TL_MODE           | \
                                         CELSIUS_DIRTY_FVF               | \
                                         CELSIUS_DIRTY_SURFACE           | \
                                         CELSIUS_DIRTY_TRANSFORM         | \
                                         CELSIUS_DIRTY_TEXTURE_TRANSFORM | \
                                         CELSIUS_DIRTY_CONTROL0          | \
                                         CELSIUS_DIRTY_COMBINERS         | \
                                         CELSIUS_DIRTY_TEXTURE_STATE     | \
                                         CELSIUS_DIRTY_LIGHTS            | \
                                         CELSIUS_DIRTY_MISC_STATE        | \
                                         CELSIUS_DIRTY_SPECFOG_COMBINER  | \
                                         CELSIUS_DIRTY_PIXEL_SHADER)

//---------------------------------------------------------------------------

// values for the texture stage <=> texture unit mappings
#define CELSIUS_UNUSED                          0xdeadbeef
#define CELSIUS_COMBINER_REG_TEX0               0x00000008      // the combiner's first texture register
                                                                // (data from first texture unit provided we always pack the tex units)

// values for texture unit status
#define CELSIUS_TEXUNITSTATUS_IDLE              0x00000000      // texture unit is unused
#define CELSIUS_TEXUNITSTATUS_USER              0x00000001      // texture unit has a real user-supplied texture
#define CELSIUS_TEXUNITSTATUS_DEFAULT           0x00000002      // texture unit has the default texture
#define CELSIUS_TEXUNITSTATUS_CLIP              0x00000004      // texture unit is being used to handle user clip plnes

#define CELSIUS_TEX_MATRIX_HACK_THRESHOLD       0.0001f         // the value that the broken element must exceed
                                                                // before the texture matrix hack will be enabled.

// general celsius state flags
#define CELSIUS_FLAG_PASSTHROUGHMODE            0x00000001      // are we in passthrough mode?
#define CELSIUS_FLAG_ZFOG                       0x00000002      // are we using z for fog?
#define CELSIUS_FLAG_TEXSTATENEEDSINVMV         0x00000004      // inverse modelview matrix is needed b/c of texture state
#define CELSIUS_FLAG_LIGHTINGNEEDSINVMV         0x00000008      // inverse modelview matrix is needed b/c of light state
#define CELSIUS_FLAG_COMBINERSNEEDSPECULAR      0x00000010      // specular is referenced as a combiner (TSS) input
#define CELSIUS_FLAG_SETUPFAILURE               0x00000020      // did we fail to give the app what it really wanted for one reason or another?
#define CELSIUS_FLAG_COMPUTE_TESS_VTX_STRIDE    0x00000040

// per-texture-unit and per-combiner state flags
#define CELSIUS_FLAG_USERTEXCOORDSNEEDED(i)     (0x00000100 << (4*(i)))  // are user texture coordinates needed by HW texture unit i?
#define CELSIUS_FLAG_PASSTHROUGHCUBEMAPPING(i)  (0x00000200 << (4*(i)))  // are we using the hack to get around broken cube maps in passthru mode?
#define CELSIUS_FLAG_TEXMATRIXHACK(i)           (0x00000400 << (4*(i)))  // are we using the hack to get around a broken texture matrix in unit i?
#define CELSIUS_FLAG_TEXMATRIXSWFIX(i)          (0x00000800 << (4*(i)))  // this stage needs a full SW work-around for the broken texture matrix
#define CELSIUS_FLAG_WDIVIDE(i)                 (0x00010000 << (4*(i)))  // is w-divide enabled in HW texture unit i?
#define CELSIUS_FLAG_WSOURCE(i)                 (0x00020000 << (4*(i)))  // can w be sourced from this unit for the purposes of w-buffering?
#define CELSIUS_FLAG_DOTPRODUCT3(i)             (0x00040000 << (4*(i)))  // this stage uses DOTPRODUCT3 as a colorop
#define CELSIUS_FLAG_ADDCOMPLEMENTRGB(i)        (0x00080000 << (4*(i)))  // this stage uses nv4's ADDCOMPLEMENT as a colorop
#define CELSIUS_FLAG_ADDCOMPLEMENTALPHA(i)      (0x01000000 << (4*(i)))  // this stage uses nv4's ADDCOMPLEMENT as an alphaop

#ifdef STOMP_TEX_COORDS
#define CELSIUS_FLAG_STOMP_4TH_COORD(i)         (0x02000000 << (4*(i)))  // stupid app fix: stomp 4th texcoord to 1.0 when they moronically send down extraneous coordinates

#define CELSIUS_FLAG_STOMP_4TH_COORD_MASK       (CELSIUS_FLAG_STOMP_4TH_COORD(0) | CELSIUS_FLAG_STOMP_4TH_COORD(1))
#endif  // STOMP_TEX_COORDS

#define CELSIUS_MASK_TEXMATRIXHACKS             (CELSIUS_FLAG_TEXMATRIXHACK(0)  | CELSIUS_FLAG_TEXMATRIXHACK(1) |   \
                                                 CELSIUS_FLAG_TEXMATRIXSWFIX(0) | CELSIUS_FLAG_TEXMATRIXSWFIX(1))

#ifdef STOMP_TEX_COORDS
#define CELSIUS_MASK_INNERLOOPUNIQUENESS        (CELSIUS_FLAG_USERTEXCOORDSNEEDED(0)    | CELSIUS_FLAG_USERTEXCOORDSNEEDED(1)    |    \
                                                 CELSIUS_FLAG_PASSTHROUGHCUBEMAPPING(0) | CELSIUS_FLAG_PASSTHROUGHCUBEMAPPING(1) |    \
                                                 CELSIUS_FLAG_TEXMATRIXHACK(0)          | CELSIUS_FLAG_TEXMATRIXHACK(1)          |    \
                                                 CELSIUS_FLAG_TEXMATRIXSWFIX(0)         | CELSIUS_FLAG_TEXMATRIXSWFIX(1)         |    \
                                                 CELSIUS_FLAG_STOMP_4TH_COORD(0)        | CELSIUS_FLAG_STOMP_4TH_COORD(1))
#else  // !STOMP_TEX_COORDS
#define CELSIUS_MASK_INNERLOOPUNIQUENESS        (CELSIUS_FLAG_USERTEXCOORDSNEEDED(0)    | CELSIUS_FLAG_USERTEXCOORDSNEEDED(1)    |    \
                                                 CELSIUS_FLAG_PASSTHROUGHCUBEMAPPING(0) | CELSIUS_FLAG_PASSTHROUGHCUBEMAPPING(1) |    \
                                                 CELSIUS_FLAG_TEXMATRIXHACK(0)          | CELSIUS_FLAG_TEXMATRIXHACK(1)          |    \
                                                 CELSIUS_FLAG_TEXMATRIXSWFIX(0)         | CELSIUS_FLAG_TEXMATRIXSWFIX(1))
#endif  // !STOMP_TEX_COORDS

#define CELSIUS_MASK_COMBINERPROGRAMUNIQUENESS  (CELSIUS_FLAG_DOTPRODUCT3(0) | CELSIUS_FLAG_DOTPRODUCT3(1) | CELSIUS_FLAG_COMBINERSNEEDSPECULAR)

#define CELSIUS_SET_NUM_TEX_COORDS(stage,num)   ((num) << (4*(stage)))
#define CELSIUS_GET_NUM_TEX_COORDS(value,stage) (((value) >> (4*(stage))) & 0xf)

// inner loop constants
#define CELSIUS_ILMASK_PRIMTYPE        0x0000ffff
#define CELSIUS_ILFLAG_LEGACY          0x00010000  // use legacy strides
#define CELSIUS_ILFLAG_CMHACK          0x00020000  // use tex matrix hack vertex copy routine
#define CELSIUS_ILFLAG_NOCULL          0x00040000  // no culling

#define CELSIUS_ILMASK_LOOPTYPE        0xf0000000  // type of inner loop
#define CELSIUS_ILFLAG_IX_VB_PRIM      0x00000000
#define CELSIUS_ILFLAG_IX_STR_PRIM     0x10000000
#define CELSIUS_ILFLAG_IX_INL_PRIM     0x20000000
#define CELSIUS_ILFLAG_IX_INL_TRI_LIST 0x40000000
#define CELSIUS_ILFLAG_IX_DVB_TRI      0x60000000
#define CELSIUS_ILFLAG_OR_VB_PRIM      0x80000000
#define CELSIUS_ILFLAG_OR_STR_PRIM     0x90000000
#define CELSIUS_ILFLAG_OR_INL_PRIM     0xa0000000
#define CELSIUS_ILFLAG_OR_INL_TRI_LIST 0xc0000000
#define CELSIUS_ILFLAG_SUPER_TRI_LIST  0xe0000000

#define CELSIUS_SHORT_INDEXED_PRIM_COUNT_CUTOFF     25    // prim length before we introduce culling
#define CELSIUS_SHORT_ORDERED_PRIM_COUNT_CUTOFF     25

// xPipe / Super-tri constants
#define CELSIUS_CULLFLAG_MODELOK       0x00000001

// xPipe / Super-tri inner loop flags. These ought to be in the inner loop flags above, but we don't have enough
// flag bits.
#define CELSIUS_STILFLAG_STDEFVB       0x00000001  // default VB is used (affects supertri logic)
#define CELSIUS_STILFLAG_MODEL         0x00000002  // Model Culling
#define CELSIUS_STILFLAG_XFORM         0x00000004  // Xform / clipcheck / cull
#define CELSIUS_STILFLAG_HIBW          0x00000008  // High bandwidth mode -- check all triangles

#ifdef WIN2K_DX7OPT
// copy of system memory vertices into hardware readable VB's
#define CELSIUS_CPFLAG_SYS2AGP         0x00000001
#define CELSIUS_CPFLAG_RESTORE         0x00000002
#define CELSIUS_CPFLAG_UPDATEMAP       0x00000004
#endif

// types
typedef void (__stdcall *CELSIUSDISPATCHPRIMITIVE)(void);

#ifdef STATE_CULL
#define CELSIUS_HWSTATE_SIZE (0x1000 >> 2)
#endif

//---------------------------------------------------------------------------

// forward definitions
class CVertexShader;

//---------------------------------------------------------------------------

// information about the state of the celsius object. these values are:
// a. shadowed from the HW so we can detect changes, or
// b. derived from several others and cached here to avoid
//    lengthy recalculation every time they're needed

class CCelsiusState
{
    //
    // intermediate state
    //
public:
#ifdef STATE_CULL
    DWORD hwState[CELSIUS_HWSTATE_SIZE];                // shadow of the hw state
    DWORD valid[CELSIUS_HWSTATE_SIZE];                  // shadow state invalidation
#endif

// bank
    DWORD          dwZEnable;                           // z-buffering? w-buffering?
    DWORD          dwSavedTexUnitToTexStageMapping[2];  // backup of mapping state so that pixel shaders can restore state when disabled
    DWORD          pfnVertexCopy;                       // routine that copies the actual vertices
    DWORD          _pad4[2];
// bank
#ifdef WIN2K_DX7OPT
    DWORD          dwSTBuff_VBOffset;                 // SuperTriangle lookaside buffer vertex buffer offset
    DWORD          dwCPFlags;                         // Copy flags
    DWORD          dw_DP2VertexLength;                // pdp2d->dwVertexLength
    DWORD          dw_DP2VertexOffset;                // pdp2d->dwVertexOffset
    DWORD          dwPromoVBOffset;
    CVertexBuffer *pPromo_DP2VertexBuffer;            // Pointer to promoted vertex buffer supplied by DP2 (or NULL)
    CVertexBuffer *pSave_VertexBufferInUse;           // vertex buffer to program in SetCelsiusState
    DWORD          _pad5[1];
#endif

//////////////////////////////////////////////////////////////////////////////
// smart celsius
//////////////////////////////////////////////////////////////////////////////

public:
    // workaround for a nv10 decode bug. fixed for all other chips, but harmless if used
    // apply before the following methods:-
    //   NV056_SET_SURFACE_CLIP_HORIZONTAL
    //   NV056_SET_SURFACE_CLIP_VERTICAL
    //   NV056_SET_SURFACE_FORMAT
    //   NV056_SET_SURFACE_PITCH
    //   NV056_SET_SURFACE_COLOR_OFFSET
    //   NV056_SET_SURFACE_ZETA_OFFSET
    //   NV056_SET_FOG_MODE
    //   NV056_SET_WINDOW_CLIP_TYPE
    //   NV056_SET_WINDOW_CLIP_HORIZONTAL(i)
    //   NV056_SET_WINDOW_CLIP_VERTICAL(i)
    __forceinline void prep_wait_for_idle_nv10 (void)
    {
        nvPushData (0,0x00040000 | (NV_DD_CELSIUS << 13) | NV056_NO_OPERATION);
        nvPushData (1,0);
        nvPusherAdjust (2);
    }
    void set (DWORD dwOffset, DWORD dwData0)
    {
#ifdef STATE_CULL
        BOOL bSend = FALSE;
        if (dwOffset & 0x1000) {
            // drawing method. must send
            bSend = TRUE;
        }
        else {
            DWORD dwIndex = (dwOffset & 0xfff) >> 2;
            if ((hwState[dwIndex+0] != dwData0) ||
                (!valid[dwIndex]))
            {
                bSend = TRUE;
                hwState[dwIndex+0] = dwData0;
                valid[dwIndex] = 1;
            }
        }
        if (bSend) 
#endif
        {
            getDC()->nvPusher.push (0, 0x00040000 | (NV_DD_CELSIUS << 13) | dwOffset);
            getDC()->nvPusher.push (1, dwData0);
            getDC()->nvPusher.adjust (2);
        }
    }
    void set2 (DWORD dwOffset, DWORD dwData0, DWORD dwData1)
    {
#ifdef STATE_CULL
        BOOL bSend = FALSE;
        if (dwOffset & 0x1000) {
            // drawing method. must send
            bSend = TRUE;
        }
        else {
            DWORD dwIndex = (dwOffset & 0xfff) >> 2;
            if ((hwState[dwIndex+0] != dwData0) ||
                (hwState[dwIndex+1] != dwData1) ||
                (!valid[dwIndex]))
            {
                bSend = TRUE;
                hwState[dwIndex+0] = dwData0;
                hwState[dwIndex+1] = dwData1;
                valid[dwIndex] = 1;
            }
        }
        if (bSend)
#endif
        {
            getDC()->nvPusher.push (0, 0x00080000 | (NV_DD_CELSIUS << 13) | dwOffset);
            getDC()->nvPusher.push (1, dwData0);
            getDC()->nvPusher.push (2, dwData1);
            getDC()->nvPusher.adjust (3);
        }
    }
    void set3 (DWORD dwOffset, DWORD dwData0, DWORD dwData1, DWORD dwData2)
    {
#ifdef STATE_CULL
        BOOL bSend = FALSE;
        if (dwOffset & 0x1000) {
            // drawing method. must send
            bSend = TRUE;
        }
        else {
            DWORD dwIndex = (dwOffset & 0xfff) >> 2;
            if ((hwState[dwIndex+0] != dwData0) ||
                (hwState[dwIndex+1] != dwData1) ||
                (hwState[dwIndex+2] != dwData2) ||
                (!valid[dwIndex]))
            {
                bSend = TRUE;
                hwState[dwIndex+0] = dwData0;
                hwState[dwIndex+1] = dwData1;
                hwState[dwIndex+2] = dwData2;
                valid[dwIndex] = 1;
            }
        }
        if (bSend)
#endif
        {
            getDC()->nvPusher.push (0, 0x000C0000 | (NV_DD_CELSIUS << 13) | dwOffset);
            getDC()->nvPusher.push (1, dwData0);
            getDC()->nvPusher.push (2, dwData1);
            getDC()->nvPusher.push (3, dwData2);
            getDC()->nvPusher.adjust (4);
        }
    }
    void set4 (DWORD dwOffset, DWORD dwData0, DWORD dwData1, DWORD dwData2, DWORD dwData3)
    {
#ifdef STATE_CULL
        BOOL bSend = FALSE;
        if (dwOffset & 0x1000) {
            // drawing method. must send
            bSend = TRUE;
        }
        else {
            DWORD dwIndex = (dwOffset & 0xfff) >> 2;
            if ((hwState[dwIndex+0] != dwData0) ||
                (hwState[dwIndex+1] != dwData1) ||
                (hwState[dwIndex+2] != dwData2) ||
                (hwState[dwIndex+3] != dwData3) ||
                (!valid[dwIndex]))
            {
                bSend = TRUE;
                hwState[dwIndex+0] = dwData0;
                hwState[dwIndex+1] = dwData1;
                hwState[dwIndex+2] = dwData2;
                hwState[dwIndex+3] = dwData3;
                valid[dwIndex] = 1;
            }
        }
        if (bSend)
#endif
        {
            getDC()->nvPusher.push (0, 0x00100000 | (NV_DD_CELSIUS << 13) | dwOffset);
            getDC()->nvPusher.push (1, dwData0);
            getDC()->nvPusher.push (2, dwData1);
            getDC()->nvPusher.push (3, dwData2);
            getDC()->nvPusher.push (4, dwData3);
            getDC()->nvPusher.adjust (5);
        }
    }

    //////////////////////////////////////////////////////////////////////////

    __forceinline void setf (DWORD dwOffset,float fData)
    {
        set (dwOffset,*(DWORD*)&fData);
    }
    __forceinline void set2f (DWORD dwOffset,float fData0,float fData1)
    {
        set2(dwOffset, *(DWORD*)&fData0, *(DWORD*)&fData1);
    }
    __forceinline void set3f (DWORD dwOffset,float fData0,float fData1,float fData2)
    {
        set3(dwOffset, *(DWORD*)&fData0, *(DWORD*)&fData1, *(DWORD*)&fData2);
    }
    __forceinline void set4f (DWORD dwOffset,float fData0,float fData1,float fData2,float fData3)
    {
        set4(dwOffset, *(DWORD*)&fData0, *(DWORD*)&fData1, *(DWORD*)&fData2, *(DWORD*)&fData3);
    }

    //////////////////////////////////////////////////////////////////////////
    // for cubemapping we need to move r into q's slot since HW can't actually see r and uses q instead
    // if the user doesn't give us a texture transform, just use a munged identity matrix
    void setTextureTransformMatrixCubeMapIdent (DWORD dwName)
    {
        set4f (dwName +  0,1.0f,0.0f,0.0f,0.0f);
        set4f (dwName + 16,0.0f,1.0f,0.0f,0.0f);
        set4f (dwName + 32,0.0f,0.0f,0.0f,0.0f);
        set4f (dwName + 48,0.0f,0.0f,1.0f,0.0f);
    }
    // if the user gave a matrix, move the third column to the 4th and transpose
    void setTextureTransformMatrixCubeMap (DWORD dwName,D3DMATRIX *pMatrix)
    {
        set4f (dwName +  0,pMatrix->_11,pMatrix->_21,pMatrix->_31,pMatrix->_41);
        set4f (dwName + 16,pMatrix->_12,pMatrix->_22,pMatrix->_32,pMatrix->_42);
        set4f (dwName + 32,0.0f,        0.0f,        0.0f,        0.0f        );
        set4f (dwName + 48,pMatrix->_13,pMatrix->_23,pMatrix->_33,pMatrix->_43);
    }

    // the form of the texture transform matrix varies depending on the number
    // of texture coords and on whether or not projection is enabled.
    // 1.) if there are two texture coordinates, then only the upper left
    // 3x3 is used. openGL and kelvin, however, always put q in the fourth position.
    // for these cases, we need to expand the 3x3 to a 4x4 and then transpose
    // 2.) if projection is disabled it is OUR reponsibility to override the
    // last column of the matrix with [0 0 0 1]' to produce q==1.0
    // type A : expand, override, and transpose
    void setTextureTransformMatrixTypeA (DWORD dwName, D3DMATRIX *pMatrix)
    {
        set4f (dwName +  0, pMatrix->_11, pMatrix->_21, 0.0f,         pMatrix->_31);
        set4f (dwName + 16, pMatrix->_12, pMatrix->_22, 0.0f,         pMatrix->_32);
        set4f (dwName + 32, 0.0f,         0.0f,         0.0f,         0.0f        );
        set4f (dwName + 48, 0.0f,         0.0f,         0.0f,         1.0f        );
    }
    // type B : override, and transpose
    void setTextureTransformMatrixTypeB (DWORD dwName, D3DMATRIX *pMatrix)
    {
        set4f (dwName +  0, pMatrix->_11, pMatrix->_21, pMatrix->_31, pMatrix->_41);
        set4f (dwName + 16, pMatrix->_12, pMatrix->_22, pMatrix->_32, pMatrix->_42);
        set4f (dwName + 32, pMatrix->_13, pMatrix->_23, pMatrix->_33, pMatrix->_43);
        set4f (dwName + 48, 0.0f,         0.0f,         0.0f,         1.0f        );
    }
    // type C : expand and transpose
    void setTextureTransformMatrixTypeC (DWORD dwName, D3DMATRIX *pMatrix)
    {
        set4f (dwName +  0, pMatrix->_11, pMatrix->_21, 0.0f,         pMatrix->_31);
        set4f (dwName + 16, pMatrix->_12, pMatrix->_22, 0.0f,         pMatrix->_32);
        set4f (dwName + 32, 0.0f,         0.0f,         0.0f,         0.0f        );
        set4f (dwName + 48, pMatrix->_13, pMatrix->_23, 0.0f,         pMatrix->_33);
    }
    // type D : just transpose
    void setTextureTransformMatrixTypeD (DWORD dwName, D3DMATRIX *pMatrix)
    {
        set4f (dwName +  0, pMatrix->_11, pMatrix->_21, pMatrix->_31, pMatrix->_41);
        set4f (dwName + 16, pMatrix->_12, pMatrix->_22, pMatrix->_32, pMatrix->_42);
        set4f (dwName + 32, 0.0f,         0.0f,         0.0f,         0.0f        );
        set4f (dwName + 48, pMatrix->_13, pMatrix->_23, pMatrix->_33, pMatrix->_43);
    }

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

    //////////////////////////////////////////////////////////////////////////

    void setMatrixIdent (DWORD dwName)
    {
        set4f (dwName +  0,1.0f,0.0f,0.0f,0.0f);
        set4f (dwName + 16,0.0f,1.0f,0.0f,0.0f);
        set4f (dwName + 32,0.0f,0.0f,1.0f,0.0f);
        set4f (dwName + 48,0.0f,0.0f,0.0f,1.0f);
    }

    //////////////////////////////////////////////////////////////////////////

public:
    inline void queueTLNOP (void)
    {
        getDC()->nvPusher.push (0,0x00040000 | (NV_DD_CELSIUS << 13) | NV056_TL_NOP);
        getDC()->nvPusher.push (1,0);
        getDC()->nvPusher.adjust (2);
    }

public:
};

// prototypes ---------------------------------------------------------------

// update all state of the celsius object
// should be called prior to rendering
HRESULT __stdcall nvSetCelsiusState        (PNVD3DCONTEXT pContext);
HRESULT           nvSetCelsiusVertexFormat (PNVD3DCONTEXT pContext);
HRESULT           nvSetCelsiusTextureState (PNVD3DCONTEXT pContext);
HRESULT           nvSetCelsiusMiscState    (PNVD3DCONTEXT pContext);

CELSIUSDISPATCHPRIMITIVE nvCelsiusGetDispatchRoutine (PNVD3DCONTEXT pConext,DWORD dwFlags);

#endif  // NVARCH >= 0x010

#endif  // _NVCELSIUSSTATE_H

