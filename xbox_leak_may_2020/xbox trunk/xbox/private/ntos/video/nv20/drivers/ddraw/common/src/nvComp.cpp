/*
 * Copyright 1993-1998 NVIDIA, Corporation.  All rights reserved.
 *
 * THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
 * NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
 * IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.
 */

/********************************* Direct 3D *******************************\
*                                                                           *
* Module: nvComp.cpp                                                        *
*   NV4 Triangle Inner Loop Compiler                                        *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Ben de Waal                 08/27/98 - Created                      *
*                                                                           *
\***************************************************************************/
#include "nvprecomp.h"

#define SPECULARPERFORMANCEFIX

#if (NVARCH >= 0x04)

/*****************************************************************************

  Inner loop strategy:

  * Inner loops are built on demand. Their uniqueness depends on the current
    render state, the platform it is run on and the currently selected FVF.
    The dispatch table associates these keys with the entry points to loops.

  * The inner loops themselves always assumes an indexed triangle list. All
    other cases generate appropriate indices on the fly

  * Singles and Quads have special cases that do not bother with the cache or
    with culling.

  * FVF cases are faster than non FVF if they have less elements - this is
    because we have unique FVF routines instead of generic ones

*****************************************************************************/

#include "x86.h"
#include "nvILHash.h"

/*
 * switches
 */
//#define INSTRUMENTED        // instrument inner loops
//#define PLOT                // plot data instead of printing it - instrumented only
//#define NOVERTEXCACHE       // do not use HW vertex cache
//#define NOLAUNCH            // do not launch triangles
#define CHECKVCTWICE        // check vertex cache in two places before missing
//#define NOSPECIALCASE       // suppress special tri & quad code paths
#define NOSKIP              // suppress skipping on prefetch
//#define NOCULLING           // suppress SW cull check
//#define PRINT_NAME          // prints ilcFlags for every primitive batch
//#define PRINT_NAME_ON_BUILD // prints name of primitive when it is built
#define NOCOMPRESSOVERHEAD  // do not use overhead compression

/*
 * flags
 */
#define MASK_FOG            0x00000003
#define MASK_PRIM           0x0000000c
#define FLAG_INDEXED        0x00000010
#define FLAG_DX6            0x00000020
#define FLAG_FIXEDWBUF      0x00000040
#define FLAG_SPECULAR       0x00000080
#define FLAG_ZFOG           0x00000100
#define FLAG_ZFP            0x00000200
#define FLAG_ZBP            0x00000400
#define FLAG_DIRECT         0x00000800 // DO NOT MODIFY - call setup or aa - depends on NV_FIXED_FUNCTION_INDEX in nvvxmac.h
#define FLAG_FLAT           0x00001000

#define FLAG_BASIC          0x00000000
#define FLAG_MMX            0x10000000
#define FLAG_KATMAI         0x20000000
#define FLAG_AMD            0x40000000

#define MASK_TABLE          0x000007ff // dwDrawPrimitiveTable is this big - we use it as a hash table

#define VD_FLAG_INDEXED     0x00000001

/*
 * macros
 */
#define ISLIST(x)           ((((x) & MASK_PRIM) == 0) || (((x) & MASK_PRIM) == 12))
#define ISSTRIP(x)          (((x) & MASK_PRIM) == 4)
#define ISFAN(x)            (((x) & MASK_PRIM) == 8)
#define ISLEGACY(x)         (((x) & MASK_PRIM) == 12)

#define ISFOGLIN(x)         (((x) & MASK_FOG) == 3)
#define ISFOGEXP(x)         (((x) & MASK_FOG) == 1)
#define ISFOGEXP2(x)        (((x) & MASK_FOG) == 2)

#define ISFLAT(x)           ((x) & FLAG_FLAT)
#define ISSPECULAR(x)       ((x) & FLAG_SPECULAR)

#define KNIMEM(x)           ((((DWORD)&((*(KATMAI_STATE*)global.kni).x[0])) + 15) & ~15)

/*
 * types
 */
typedef (__stdcall *PFNINNERLOOP)(void);

/*
 * aliases
 */
#define ilcData         global.dwILCData
#define ilcCount        global.dwILCCount
#define ilcMax          global.dwILCMax

#ifdef VTUNE_SUPPORT
void nvVTuneMemoryImage        (void);
void nvVTuneModifyAccessRights (void);
#endif //VTUNE_SUPPORT

/*
 * short-lived globals
 */
DWORD ilcFlags;
DWORD label_cache;

#ifdef DEBUG

#if 0 // DO NOT DELETE - valuable z-buffer blt code
static DWORD table[256];
static DWORD max = 0;

DWORD __inline mapZtoColor (DWORD c)
{
/*
    double t = ((double)c) / ((double)0xffffffff);
    double r = (t > 0.5) ? sin((t - .5) * .5 * 3.141592653) : 0.0;
    double g = sin(t * 3.141592653);
    double b = (t < .5 ) ? cos(t * .5 * 3.141592653) : 0.0;

    return (((DWORD)(r * 255.0)) << 16)
         | (((DWORD)(g * 255.0)) <<  8)
         | (((DWORD)(b * 255.0)) <<  0);
*/

    static DWORD color = 0x40;

    DWORD i;
    for (i = 0; i < max; i += 2)
    {
        if (table[i] == c)
        {
            return table[i+1];
        }
    }

    table[max]     = c;
    table[max + 1] = color;

    max += 2;
    color += 0x40;

    return table[max-1];
}
#endif

void __stdcall ILCDebug_flushTriangle (void)
{
#ifndef NV_NULL_HW_DRIVER
    getDC()->nvPusher.setPut(global.nvCachedPut);
#endif
    dbgFlushType (NVDBG_FLUSH_PRIMITIVE);
    global.nvCachedPut       = getDC()->nvPusher.getPut();
    global.nvCachedThreshold = getDC()->nvPusher.getThreshold();

#if 0 // DO NOT DELETE - valuable z-buffer blt code
    {
        DWORD y;
        DWORD x;
        DWORD line  = pContext->dwSurfaceAddr + (320 / 2) * 4;
        DWORD zline = pContext->ZBufferAddr;

        for (y = 0; y < 280; y++)
        {
            DWORD addr = line;
            DWORD zaddr = zline;

            for (x = 0; x < 320; x++)
            {
                *(DWORD*)addr = mapZtoColor(*(DWORD*)zaddr);
                addr += 4;
                zaddr += 4;
            }

            line  += pContext->surfacePitch.wColorPitch;
            zline += pContext->surfacePitch.wZetaPitch;
        }

    }
    {
        DWORD i;

        DPF ("table:");
        for (i = 0; i < max; i += 2)
        {
            DPF ("%08x -> %08x",table[i],table[i+1]);
        }
    }
    __asm int 3;
#endif
}
#endif // DEBUG

#if defined(DEBUG) || defined(NVSTATDRIVER)
void __stdcall ILCDebug_showVertex (DWORD *pdwAddr)
{
    dbgDisplayVertexData (FALSE, pdwAddr);
}

#endif

#ifdef INSTRUMENTED
/*
 * ILCDebug_plot
 *
 * plot a pixel
 */
void ILCDebug_plot
(
    int x,
    int y,
    int c
)
{
    DWORD addr = 0xb0000 + 0x2000 * (y & 3) + 90 * (y / 4) + (x / 8);
    DWORD bit  = 0x80 >> (x & 7);

    switch (c)
    {
        case 0: *(BYTE*)addr |= bit;
                break;
        case 1: if ((x & 1) ^ (y & 1)) *(BYTE*)addr |= bit;
                                  else *(BYTE*)addr &= ~bit;
                break;
        case 2: if (!((x & 1) && (y & 1))) *(BYTE*)addr |= bit;
                                      else *(BYTE*)addr &= ~bit;
                break;
    }
}

/*
 * ILCDebug_hline
 *
 * draw a line
 */
void ILCDebug_hline
(
    int y
)
{
    DWORD addr = 0xb0000 + 0x2000 * (y & 3) + 90 * (y / 4);
    int x;
    for (x = 0; x < 720; x += 8)
    {
        *(BYTE*)addr |= 0x07;
        addr++;
    }
}

/*
 * ILCDebug_vline
 *
 * draw a line
 */
void ILCDebug_vline
(
    int x
)
{
    int y;
    for (y = 0; y < 348; y += 4)
    {
        ILCDebug_plot (x,y,0);
    }
}

/*
 * ILCDebug_cls
 *
 * set graphics mode and clear debug monitor
 */
void ILCDebug_cls
(
    void
)
{
    static int program[] =
    {
        0x3bf,1,
        0x3b8,0,
        0x3b4,0,  0x3b5,0x35,
        0x3b4,1,  0x3b5,0x2d,
        0x3b4,2,  0x3b5,0x2e,
        0x3b4,3,  0x3b5,0x07,
        0x3b4,4,  0x3b5,0x5b,
        0x3b4,5,  0x3b5,0x02,
        0x3b4,6,  0x3b5,0x57,
        0x3b4,7,  0x3b5,0x57,
        0x3b4,8,  0x3b5,0x02,
        0x3b4,9,  0x3b5,0x03,
        0x3b4,10, 0x3b5,0x00,
        0x3b4,11, 0x3b5,0x00,
        0x3b8,0x2a  // 0xaa for page 1
    };

    /*
     * set mode
     */
    {
        int i;

        for (i = 0; i < sizeof(program) / sizeof(int); i += 2)
        {
            _outp ((WORD)program[i],(BYTE)program[i+1]);
        }
    }

    /*
     * cls
     */
    memset ((void*)0xb0000,0,32768);

    /*
     * draw grid
     */
    {
        float i;

        for (i = 0.0f; i < 348.0f; i += 348.0f / 8.0f) // 100Mb / div
        {
            ILCDebug_hline (347 - (int)i);
        }
        for (i = 0.0f; i < 720.0f; i += 720.0f / 4.0f) // 16kb / div
        {
            ILCDebug_vline ((int)i);
        }
    }
}

/*
 * ILCDebug_inspectXMM
 *
 * dumps xmm state so I can se what it is doing
 */
static DWORD xmmState[4*8];
void __stdcall ILCDebug_inspectXMM
(
    DWORD dwLine
)
{
    for (DWORD i=0; i < 8; i++) {
        DPF ("XMM%d: %08x %08x %08x %08x", i,xmmState[i*4+3],xmmState[i*4+2],xmmState[i*4+1],xmmState[i*4+0]);
    }
    DPF ("Line %d", dwLine);
}

#define xBREAK_XMM                                       \
{                                                        \
    xMOVUPS_i128_r ((DWORD)(xmmState +  0),rXMM0)        \
    xMOVUPS_i128_r ((DWORD)(xmmState +  4),rXMM1)        \
    xMOVUPS_i128_r ((DWORD)(xmmState +  8),rXMM2)        \
    xMOVUPS_i128_r ((DWORD)(xmmState + 12),rXMM3)        \
    xMOVUPS_i128_r ((DWORD)(xmmState + 16),rXMM4)        \
    xMOVUPS_i128_r ((DWORD)(xmmState + 20),rXMM5)        \
    xMOVUPS_i128_r ((DWORD)(xmmState + 24),rXMM6)        \
    xMOVUPS_i128_r ((DWORD)(xmmState + 28),rXMM7)        \
    xPUSHAD                                              \
    xPUSH_imm   (__LINE__)                               \
    xMOV_rm_imm (rmREG(rEAX),(DWORD)ILCDebug_inspectXMM) \
    xCALL_rm    (rmREG(rEAX))                            \
    xPOPAD                                               \
    xINT3                                                \
}

#endif  // INSTRUMENTED

#ifdef DEBUG

/*
 * ILCDebug_printLine
 *
 * debug helper to print break point line to debugger
 */
void __stdcall ILCDebug_printLine
(
    DWORD dwLine
)
{
    DPF ("Line %d", dwLine);
}
#define xBREAK { xPUSHAD xMOV_rm_imm (rmREG(rEBX),(DWORD)ILCDebug_printLine) xPUSH_imm (__LINE__) xCALL_rm (rmREG(rEBX)) xPOPAD xINT3 }

#endif

#if defined(PRINT_NAME) || defined(PRINT_NAME_ON_BUILD)
/*
 * ILCDebug_printName
 *
 * prints the current innerloop name
 */
void ILCDebug_printName
(
    DWORD dwFlags,
    DWORD dwVertexType,
    DWORD dwTextureOrder
)
{
    DPF ("flags=%08x fvf=%08x order=%08x", dwFlags,dwVertexType,dwTextureOrder);
}
#endif

/*
 * ILCCompile_mul
 *
 * multiplies the given register with the given constant
 *  tries to be cycle smart
 */
void ILCCompile_mul
(
    DWORD reg,
    DWORD num
)
{
    DWORD start = ilcCount;
    DWORD startnum = num;
    DWORD ops   = 0;

    // this gets very unhappy if num=0
    nvAssert (num != 0);

    for (;;)
    {
        /*
         * multiple of 9
         */
        if (((num / 9) * 9) == num)
        {
            xLEA_r_rm (reg,rmSIB) xSIB(reg,reg,x8)
            num /= 9;
            ops ++;
            continue;
        }

#if 0
        /*
         * multiple of 7
         */
        if (((num / 7) * 7) == num)
        {
            DWORD r2 = (reg == rEAX) ? rEBX : rEAX;
            xPUSH_r     (r2)
            xMOV_r_rm   (r2,rmREG(reg))
            xSHL_rm_imm8(rmREG(reg),3)  // n*8
            xSUB_r_rm   (reg,rmREG(r2)) // n*8-n
            xPOP_r      (r2)
            num /= 7;
            ops += 5;
            continue;
        }
#endif

        /*
         * multiple of 5
         */
        if (((num / 5) * 5) == num)
        {
            xLEA_r_rm (reg,rmSIB) xSIB(reg,reg,x4)
            num /= 5;
            ops ++;
            continue;
        }

        /*
         * multiple of 3
         */
        if (((num / 3) * 3) == num)
        {
            xLEA_r_rm (reg,rmSIB) xSIB(reg,reg,x2)
            num /= 3;
            ops ++;
            continue;
        }

        /*
         * power of 2, but not 1
         */
        if ((num > 1) && !(num & (num - 1)))
        {
            DWORD log = ~0;
            while (num) { log ++; num >>= 1; }
            xSHL_rm_imm8(rmREG(reg),log)
            ops ++;
            num = 1;
            break;
        }

        /*
         * all others
         */
        break;
    }

    /*
     * if it took too many ops or if we could not
     *  decompose it, replace with IMUL
     */
    if ((num != 1) || (ops >= 5))
    {
        ilcCount = start;
        xIMUL_r_r_imm (reg,reg,startnum)
    }
}

#ifdef WINNT
/*
 * ILCCompile_checkValid
 *
 * check if index is still valid (NT requirement)
 */
void ILCCompile_checkValid
(
    DWORD reg
)
{
    DWORD label;

    xCMP_r_i32  (reg,mMEM32(global.dwMaxVertexOffset))
    xLABEL      (label)
    xJL         (0)

    xXOR_r_rm   (reg,rmREG(reg))

    xTARGET_b8  (label)
}
#endif

/*
 * calculate fog values
 *  KATMAI uses SIMD to do the whole triangle at once
 *  - after computing fog it leaves the results in dwFogValue ready to be
 *    used by subsequent vertex copy cycles
 *  - if we have the special quad case, we need to do four verts at a time
 *  ALL OTHERS uses the slow per-vertex method described later
 */
void ILCCompile_KatmaiFog
(
    BOOL  bQuadCase
)
{
    DWORD offset = (ilcFlags & FLAG_ZFOG) ? 8 : 12; // z or rhw

    // get z or rhw values for the three vertices
    xMOV_r_i32 (rEAX,mMEM32(global.dwVertex0))
     xMOV_r_i32 (rEBX,mMEM32(global.dwVertex1))
      xMOV_r_i32 (rECX,mMEM32(global.dwVertex2))
       if (bQuadCase)
       {
            xMOV_r_i32 (rEDX,mMEM32(global.pVertices))
       }
    xMOV_r_rm  (rEAX,rmIND8(rEAX)) xOFS8 (offset)
     xMOV_r_rm  (rEBX,rmIND8(rEBX)) xOFS8 (offset)
      xMOV_r_rm  (rECX,rmIND8(rECX)) xOFS8 (offset)
       if (bQuadCase)
       {
            xMOV_r_rm  (rEDX,rmIND8(rEDX)) xOFS8 (offset)
       }
    xMOV_i32_r (KNIMEM(fRHW) + 0,rEAX)
     xMOV_i32_r (KNIMEM(fRHW) + 4,rEBX)
      xMOV_i32_r (KNIMEM(fRHW) + 8,rECX)
       if (bQuadCase)
       {
            xMOV_i32_r (KNIMEM(fRHW) + 12,rEDX)
       }

    // get xmm0 = ZFOG ? z : (1 / rhw)
    xMOVAPS_r_i128 (rXMM0,KNIMEM(fRHW))
    if (!(ilcFlags & FLAG_ZFOG))
    {
        xRCPPS_r_rm (rXMM0,rmREG(rXMM0)) // w <= (1 / rhw)
    }

    if (ISFOGLIN(ilcFlags))
    {
        // linear fog, scale: xmm1 = (end - xmm0) * scale
        xMOVAPS_r_i128 (rXMM1,KNIMEM(fFogTableEnd))
        xSUBPS_r_rm    (rXMM1,rmREG(rXMM0))
        xMULPS_r_i128  (rXMM1,KNIMEM(fFogTableLinearScale))

        // clamp
//        xMAXPS_r_i128  (rXMM1,KNIMEM(fZero))
//        xMINPS_r_i128  (rXMM1,KNIMEM(fOne))
    }
    else
    {
        // exp and exp2 fog, scale: xmm0 *= density
        xMULPS_r_i128  (rXMM0,KNIMEM(fFogTableDensity))

        // exp2 muls with itself, xmm0 *= xmm0
        if (ISFOGEXP2(ilcFlags))
        {
            xMULPS_r_rm (rXMM0,rmREG(rXMM0))
        }

        // xmm1 = approx(e ^ xmm0)
        xMOV_rm_imm    (rmREG(rEBX),0x3f800000)
        xMULPS_r_i128  (rXMM0,KNIMEM(fFogC2))
        xCVTPS2PI_r_rm (rMM0,rmREG(rXMM0))
        xMOVHLPS_r_r   (rXMM0,rmREG(rXMM0))
        xCVTPS2PI_r_rm (rMM1,rmREG(rXMM0))
        xMOVQ_i64_r    (KNIMEM(dwFogValue) + 0,rMM0)
        xMOVQ_i64_r    (KNIMEM(dwFogValue) + 8,rMM1)
        xADD_i32_r     (KNIMEM(dwFogValue) + 0,rEBX)
        xADD_i32_r     (KNIMEM(dwFogValue) + 4,rEBX)
        xADD_i32_r     (KNIMEM(dwFogValue) + 8,rEBX)
        if (bQuadCase)
        {
            xADD_i32_r (KNIMEM(dwFogValue) + 12,rEBX)
        }
        xMOVAPS_r_i128 (rXMM1,KNIMEM(dwFogValue))
    }

    // convert to 8 bit integer
    xMULPS_r_i128  (rXMM1,KNIMEM(f255))
    xMAXPS_r_i128  (rXMM1,KNIMEM(fZero))
    xMINPS_r_i128  (rXMM1,KNIMEM(f255))
    xCVTPS2PI_r_rm (rMM0,rmREG(rXMM1))
    xMOVHLPS_r_r   (rXMM1,rmREG(rXMM1))
    xCVTPS2PI_r_rm (rMM1,rmREG(rXMM1))
    xMOVQ_i64_r    (KNIMEM(dwFogValue) + 0,rMM0)
    xMOVQ_i64_r    (KNIMEM(dwFogValue) + 8,rMM1)
    xEMMS
}

/*
 * ILCCompile_vertex
 *
 * copy vertex into push buffer
 *  ebx has hw address to load into when pos == ~0
 *  if pos == (~0 - 1) then we will not load the hw address (caller must do it)
 *  if pos == 0x8000000? then we plan to load them amount of vertices specified
 *  in the low nybble. 0x80000001 has the same effect as ~0.
 *  vertex must be in ebx
 */
void ILCCompile_vertex
(
    PNVD3DCONTEXT pContext,
    DWORD vertexNumber,     // 0,1,2 or 3 for quad
    DWORD vertexAddress,
    DWORD pos,
    BOOL  bAllowVertexCache,
    DWORD dwArbitrationLevel,
    BOOL  bQuadCase
)
{
    PNVD3DMULTITEXTURESTATE pTriangleState = (PNVD3DMULTITEXTURESTATE)&pContext->mtsState;
    DWORD index;
    BOOL  bRHW        = FALSE;
    DWORD size        = (ilcFlags & FLAG_DX6) ? 11 : 9;
    BOOL  bSkipHWAddr = (pos == 0xfffffffe);

    // get pusher address
    if (bAllowVertexCache)
    {
        xPUSH_r    (rEDI)
        xMOV_r_i32 (rEDI,mMEM32(global.nvCachedPut))
    }
    else
    {
        if (dwArbitrationLevel == 0)
        {
            xMOV_r_i32 (rEDI,mMEM32(global.nvCachedPut))
        }
    }

    if ((dwArbitrationLevel == 0)
     && (ilcFlags & MASK_FOG)
     && (ilcFlags & FLAG_KATMAI))
    {
        if (pos == ~0)
        {
            xPUSH_r (rEBX)
        }
        ILCCompile_KatmaiFog(bQuadCase);
        if (pos == ~0)
        {
            xPOP_r (rEBX)
        }
    }

    // load method into ebx
    if (bSkipHWAddr)
    {
        xLEA_r_rm (rEDI,rmIND8(rEDI)) xOFS8(-4) // spoof offsets to make up for missing header
    }
    else
    {
        if ((pos == ~0) || ((pos & ~0xf) == 0x80000000))
        {
            pos = (pos == ~0) ? 1 : (pos & 15);
            if (ilcFlags & FLAG_DX6)
            {
                DWORD or = 0xe400 | ((40 * pos) << 16); // dx6 class specific
                ILCCompile_mul (rEBX,40);
                xOR_rm_imm  (rmREG(rEBX),or)
            }
            else
            {
                DWORD or = 0xe400 | ((32 * pos) << 16); // dx5 class specific
                ILCCompile_mul (rEBX,32);
                xOR_rm_imm  (rmREG(rEBX),or)
            }
        }
        else
        {
            xMOV_rm_imm (rmREG(rEBX),((ilcFlags & FLAG_DX6) ? (0x0028e400 | pos * 40)
                                                            : (0x0020e400 | pos * 32)))
        }
    }

    // point to vertex
    xMOV_r_i32 (rEDX,vertexAddress)

    /*------------------------------------------------------------------------
     * special cases
     *-----------------------------------------------------------------------*/

    if ( (pContext->pCurrentVShader->getFVF() == D3DFVF_TLVERTEX)
     &&  (ilcFlags & FLAG_KATMAI)
     && !(ilcFlags & (FLAG_DX6 | MASK_FOG | FLAG_FLAT | FLAG_FIXEDWBUF | FLAG_ZFOG | FLAG_ZFP | FLAG_ZBP)))
    {
        // P3 DX5 TL vertex
        xMOVLPS_r_rm (rXMM0,rmIND (rEDX))
        xMOVHPS_r_rm (rXMM0,rmIND8 (rEDX)) xOFS8(8)
        xMOVLPS_r_rm (rXMM1,rmIND8(rEDX)) xOFS8(16)
        xMOVHPS_r_rm (rXMM1,rmIND8(rEDX)) xOFS8(24)
        if (!bSkipHWAddr)
        {
            xMOV_rm_r    (rmIND (rEDI),rEBX)
        }
        xMOVLPS_rm_r (rmIND8(rEDI),rXMM0) xOFS8( 4)
        xMOVHPS_rm_r (rmIND8(rEDI),rXMM0) xOFS8( 12)
        xMOVLPS_rm_r (rmIND8(rEDI),rXMM1) xOFS8(20)
        xMOVHPS_rm_r (rmIND8(rEDI),rXMM1) xOFS8(28)
        goto vertexDone;
    }

    /*------------------------------------------------------------------------
     * generic case
     *-----------------------------------------------------------------------*/

    // read x,y,z,[w]
    if (pContext->pCurrentVShader->getFVF() & D3DFVF_XYZRHW)
    {
        xMOV_r_rm (rECX,rmIND(rEDX))
        xMOV_r_rm (rEBP,rmIND8(rEDX)) xOFS8 (4)
        xMOV_r_rm (rEAX,rmIND8(rEDX)) xOFS8 (8)
    }
    else
    {
        xXOR_r_rm (rECX,rmREG(rECX))        // default x = 0.0f
        xXOR_r_rm (rEBP,rmREG(rEBP))        // default y = 0.0f
        xXOR_r_rm (rEAX,rmREG(rEAX))        // default z = 0.0f
    }

    // write method, x,y,z,[w]
    if (!bSkipHWAddr)
    {
        xMOV_rm_r (rmIND(rEDI),rEBX)
    }
    xMOV_rm_r (rmIND8(rEDI),rECX) xOFS8 (4)
    xMOV_rm_r (rmIND8(rEDI),rEBP) xOFS8 (8)

    // write z
    if (ilcFlags & (FLAG_ZFP | FLAG_ZBP))
    {
        xPUSH_r (rEAX)

        if (ilcFlags & FLAG_ZFP)
        {
            // clamp z to 1.0f
            xCMP_rm_imm  (rmREG(rEAX),0x3f800000)
            xSETG_rm8    (rmREG(rBL))
            xSHR_rm_imm8 (rmREG(rEBX),1)
            xSBB_r_rm    (rEBX,rmREG(rEBX))     // ebx = (eax > 1.0f) ? -1 : 0

            xMOV_r_rm    (rECX,rmREG(rEBX))
            xNOT_rm      (rmREG(rECX))          // ecx = (eax > 1.0f) ? 0 : -1

            xAND_rm_imm  (rmREG(rEBX),0x3f800000)
            xAND_r_rm    (rEAX,rmREG(rECX))
            xOR_r_rm     (rEAX,rmREG(rEBX))
        }

        if (ilcFlags & FLAG_ZBP)
        {
            /*
             * clamp z to 0.0f
             */
            xMOV_r_rm    (rEBX,rmREG(rEAX))
            xSHL_rm_imm8 (rmREG(rEBX),1)
            xSBB_r_rm    (rEBX,rmREG(rEBX))     // ebx = (eax <= -0.0f) ? -1 : 0
            xNOT_rm      (rmREG(rEBX))          // ebx = (eax <= -0.0f) ? 0 : -1
            xAND_r_rm    (rEAX,rmREG(rEBX))
        }

        xMOV_i32_r   (mMEM32(global.dwTemp),rEAX)

        /*
         * optimize
         */
        xFLD_i32  (mMEM32(global.dwTemp))
        xFMUL_i32 (mMEM32(global.fCTC1));
        if (ilcFlags & FLAG_ZBP)
        {
            xFLD1
            xFSUBRP_st (rST1);
        }
        xFSTP_rm  (rmIND8(rEDI)) xOFS8 (12)

        xPOP_r (rEAX)
    }
    else
    {
        xMOV_rm_r (rmIND8(rEDI),rEAX) xOFS8 (12)
    }

    /*
     * read rhw
     */
    index = 12;
    if (pContext->pCurrentVShader->getFVF() & D3DFVF_XYZRHW)
    {
        xMOV_r_rm (rEBX,rmIND8(rEDX)) xOFS8 (index) index += 4;
    }
    else
    {
        xMOV_rm_imm (rmREG(rEBX), FP_ONE_BITS) // default rhw == 1.0f
    }

    /*
     * read color
     */
    if (pContext->pCurrentVShader->getFVF() & D3DFVF_DIFFUSE)
    {
        if (ilcFlags & FLAG_FLAT)
        {
            xMOV_r_i32 (rECX,mMEM32(global.dwFlatColor)) index += 4;
        }
        else
        {
            xMOV_r_rm  (rECX,rmIND8(rEDX)) xOFS8 (index) index += 4;
        }
    }
    else
    {
        xMOV_rm_imm (rmREG(rECX),0xffffffff) // default color == rgba(1,1,1,1)
    }

    /*
     * read specular
     */
    if (pContext->pCurrentVShader->getFVF() & D3DFVF_SPECULAR)
    {
        xMOV_r_rm (rEBP,rmIND8(rEDX)) xOFS8 (index) index += 4;

        /*
         * apply flat specular
         */
        if (ilcFlags & FLAG_FLAT)
        {
            xAND_rm_imm (rmREG(rEBP),0xff000000)
            xOR_r_i32   (rEBP,mMEM32(global.dwFlatSpecular))
        }
    }
    else
    {
        xMOV_rm_imm (rmREG(rEBP),0xff000000) // default specular == rgba(0,0,0,1)
    }

    /*
     * apply fog
     *  KATMAI uses already prepared values
     *  ALL OTHERS uses the slow per-vertex method
     *  - it uses z which is in eax or rhw which is in ebx to finally
     *    modify specular in ebp.
     */
    if (ilcFlags & MASK_FOG)
    {
        if (ilcFlags & FLAG_KATMAI)
        {
            /*
             * incorporate fog into specular component
             */
            xMOV_r_i32   (rEAX,KNIMEM(dwFogValue) + 4 * vertexNumber)
            xAND_rm_imm  (rmREG(rEBP),0x00ffffff)
            xSHL_rm_imm8 (rmREG(rEAX),24)
            xOR_r_rm     (rEBP,rmREG(rEAX))
        }
        else
        {
            /*
             * generic case - compute fog value
             */
            BOOL isExp2 = ISFOGEXP2(ilcFlags);
            BOOL isLin  = ISFOGLIN(ilcFlags);

            /*
             * save RHW and free up ebx for general use
             */
            xPUSH_r      (rEBX)
            xPUSH_r      (rEDX)
            xMOV_i32_r   (KNIMEM(fRHW),rEBX)
            bRHW = TRUE;

            /*
             * st(0) = (global.dvWNear == 1.0f) ? z : approx(1.0 / rhw)
             */
            if (ilcFlags & FLAG_ZFOG)
            {
                xMOV_i32_r   (mMEM32(global.dwTemp),rEAX)   // z - eax is free now
                xFLD_i32     (mMEM32(global.dwTemp))
            }
            else
            {
                xNEG_rm      (rmREG(rEBX))
                xADD_rm_imm  (rmREG(rEBX),0x3f800000*2)     // 2 * asint(1.0f) - rhw
                xMOV_i32_r   (mMEM32(global.dwTemp),rEBX)
                xFLD_i32     (mMEM32(global.dwTemp))
                xFLD_st      (rST0)
                xFMUL_i32    (KNIMEM(fRHW))
                xFLD_i32     (KNIMEM(fFogC1))
                xFSUBRP_st   (rST1)
                xFMULP_st    (rST1)
            }

            if (isLin)
            {
                DWORD fix11,fix12;
                DWORD fix21,fix22;

                /*
                 * (st(0) >= pContext->fogData.fFogTableEnd) ? 0.0f
                 */
                xFCOM_i32  (KNIMEM(fFogTableEnd))
                xFNSTSW
                xSAHF
                xLABEL     (fix11)
                xJB        (0)

                xFFREE_st  (rST0)
                xFINCSTP
                xFLDZ

                xLABEL     (fix12)
                xJMP       (0)

                /*
                 * else (st(1) <= pContext->fogData.fFogTableStart) ? 1.0f
                 */
                xTARGET_b8 (fix11)

                xFCOM_i32  (KNIMEM(fFogTableStart))
                xFNSTSW
                xSAHF
                xLABEL     (fix21)
                xJA        (0)

                xFFREE_st  (rST0)
                xFINCSTP
                xFLD1

                xLABEL     (fix22)
                xJMP       (0)


                /*
                 * else st(0) = (fFogTableEnd - st(0)) * fFogTableLinearScale
                 */
                xTARGET_b8 (fix21)

                xFLD_i32   (KNIMEM(fFogTableEnd))
                xFSUBRP_st (rST1)
                xFMUL_i32  (KNIMEM(fFogTableLinearScale))

                xTARGET_jmp (fix12)
                xTARGET_jmp (fix22)
            }
            else
            {
                /*
                 * st(0) *= fFogTableDensity
                 */
                xFMUL_i32  (KNIMEM(fFogTableDensity))

                if (isExp2)
                {
                    /*
                     * st(0) *= st(0)
                     */
                    xFMUL_st (rST0)
                }

                /*
                 * st(0) = exp( st(0) )
                 */
                xMOV_rm_imm (rmREG(rEBX),0x3f800000)
                xFMUL_i32   (KNIMEM(fFogC2))
                xFISTP_i32  (mMEM32(global.dwTemp))
                xADD_i32_r  (mMEM32(global.dwTemp),rEBX)
                xFLD_i32    (mMEM32(global.dwTemp))
            }

            /*
             * eax = int8bits ( st(0) ) << 24
             */
            xFLD1
            xFADDP_st    (rST1)
            xFSTP_i32    (mMEM32(global.dwTemp))
            xMOV_r_i32   (rEBX,mMEM32(global.dwTemp))

            xCMP_rm_imm  (rmREG(rEBX),0x3f800000)
            xSETG_rm8    (rmREG(rDL))
            xSHR_rm_imm8 (rmREG(rEDX),1)
            xSBB_r_rm    (rEAX,rmREG(rEAX))

            xCMP_rm_imm  (rmREG(rEBX),0x40000000)
            xSETGE_rm8   (rmREG(rDL))
            xSHR_rm_imm8 (rmREG(rEDX),1)
            xSBB_r_rm    (rEDX,rmREG(rEDX))

            xSHL_rm_imm8 (rmREG(rEBX),9)
            xOR_r_rm     (rEBX,rmREG(rEDX))
            xAND_r_rm    (rEBX,rmREG(rEAX))
            xAND_rm_imm  (rmREG(rEBX),0xff000000)

            /*
             * update specular
             */
            xAND_rm_imm  (rmREG(rEBP),0x00ffffff)
            xOR_r_rm     (rEBP,rmREG(rEBX))

            xPOP_r       (rEDX)
            xPOP_r       (rEBX)
        }
    }

    /*
     * apply fixed w-buffer scale to ebx
     */
    if (ilcFlags & FLAG_FIXEDWBUF)
    {
        if (!bRHW)
        {
            xMOV_i32_r   (KNIMEM(fRHW),rEBX)
            bRHW = TRUE;
        }

        xFLD_i32   (KNIMEM(fRHW))
        xFMUL_i32  ((DWORD)&global.fRWFar)
        xFSTP_i32  (KNIMEM(fRHW))
        xMOV_r_i32 (rEBX,KNIMEM(fRHW))
    }

    /*
     * read tu0
     */
    if (pContext->pCurrentVShader->bVAExists(defaultInputRegMap[D3DVSDE_TEXCOORD0]))
    {
        DWORD ofs = index + pTriangleState->dwUVOffset[0];
        xMOV_r_rm (rEAX,rmIND8(rEDX)) xOFS8 (ofs)
    }
    else
    {
        xXOR_r_rm (rEAX,rmREG(rEAX))        // default tu0 = 0.0f
    }

    /*
     * write rhw, color, specular, tu0
     */
    xMOV_rm_r (rmIND8(rEDI),rEBX) xOFS8 (16)
    xMOV_rm_r (rmIND8(rEDI),rECX) xOFS8 (20)
    xMOV_rm_r (rmIND8(rEDI),rEBP) xOFS8 (24)
    xMOV_rm_r (rmIND8(rEDI),rEAX) xOFS8 (28)

    /*
     * read tv0
     */
    if (pContext->pCurrentVShader->bVAExists(defaultInputRegMap[D3DVSDE_TEXCOORD0]))
    {
        DWORD ofs = index + pTriangleState->dwUVOffset[0] + 4;
        xMOV_r_rm (rEBX,rmIND8(rEDX)) xOFS8 (ofs)
    }
    else
    {
        xXOR_r_rm (rEBX,rmREG(rEBX))        // default tv0 = 0.0f
    }

    /*
     * read tu1, tv1
     */
    if (ilcFlags & FLAG_DX6)
    {
        DWORD dwTCIndex = pTriangleState->dwUVOffset[1] / (2*sizeof(D3DVALUE));
        DWORD dwVAIndex = defaultInputRegMap[D3DVSDE_TEXCOORD0+dwTCIndex];
        if (pContext->pCurrentVShader->bVAExists (dwVAIndex))
        {
            DWORD ofs = index + pTriangleState->dwUVOffset[1];
            xMOV_r_rm (rECX,rmIND8(rEDX)) xOFS8 (ofs)
            xMOV_r_rm (rEBP,rmIND8(rEDX)) xOFS8 (ofs + 4)
        }
        else
        {
            xXOR_r_rm (rECX,rmREG(rECX))        // default tu1 = 0.0f
            xXOR_r_rm (rEBP,rmREG(rEBP))        // default tv1 = 0.0f
        }
    }

    /*
     * write tv0, tu1, tv1
     */
    xMOV_rm_r (rmIND8(rEDI),rEBX) xOFS8 (32)
    if (ilcFlags & FLAG_DX6)
    {
        xMOV_rm_r (rmIND8(rEDI),rECX) xOFS8 (36)
        xMOV_rm_r (rmIND8(rEDI),rEBP) xOFS8 (40)
    }

vertexDone:
#if defined(DEBUG) || defined(NVSTATDRIVER)
    xPUSHAD
    xMOV_rm_imm (rmREG(rEBX),(DWORD)ILCDebug_showVertex)
    xLEA_r_rm   (rEAX,rmIND8(rEDI)) xOFS8(4)
    xPUSH_r     (rEAX)
    xCALL_rm    (rmREG(rEBX))
    xPOPAD
#endif

    /*
     * update pusher
     */
    xLEA_r_rm  (rEDI,rmIND8(rEDI)) xOFS8(size * 4) // update pusher base

    /*
     * done
     */
    if (bAllowVertexCache)
    {
        xMOV_i32_r (mMEM32(global.nvCachedPut),rEDI)
        xPOP_r     (rEDI)
    }
    else
    {
        if (dwArbitrationLevel == 2)
        {
            xMOV_i32_r (mMEM32(global.nvCachedPut),rEDI)
        }
    }
}

/*
 * ILCCompile_testCache
 *
 * test if the given index is in the vertex cache
 *   the index is used and if the cache is missed we branch to label.
 *   this code expects ILCCompile_missCache to reside at label
 *  - ecx has HW vc index on exit
 *  - ebx must have the post-arbitration index loaded on entry
 */
void ILCCompile_testCache
(
    DWORD indexAddress,
    DWORD label
)
{
    /*
     * get original index, or vc serial number and test for a cache hit
     */
    xMOV_r_i32   (rEAX,indexAddress)
    xOR_r_i32    (rEAX,mMEM32(global.dwVertexSN))
    xCMP_r_rm    (rEAX,rmSIB) xSIBID(rEBX,x4) xOFS32 (mMEM32(global.dwVertexCache)) // cmp eax,[vc + ebx*4]
    xJNZ32       (label)
    xMOV_r_rm    (rECX,rmREG(rEBX))
#ifdef CHECKVCTWICE
    xTARGET_b32  (label_cache)
#endif
}

/*
 * ILCCompile_missCache
 *
 * finish work on a cache miss. see ILCCompile_testCache
 * ebx has HW vertex location
 */
void ILCCompile_missCache
(
    void
)
{
    /*
     * setup cache variables
     */
#ifdef CHECKVCTWICE
    DWORD cacheSize = (ilcFlags & FLAG_DX6) ? 7 : 15;
    DWORD arbAdd    = (ilcFlags & FLAG_DX6) ? 3 : 7;
#endif

    /*
     * see if we missed because of arbitration (2% likelyhood)
     */
#ifdef CHECKVCTWICE
    xMOV_r_rm   (rECX,rmREG(rEBX))
    xADD_rm_imm (rmREG(rECX),arbAdd)
    xAND_rm_imm (rmREG(rECX),cacheSize)
    xCMP_r_rm   (rEAX,rmSIB) xSIBID(rECX,x4) xOFS32 (mMEM32(global.dwVertexCache)) // cmp eax,[vc + ecx*4]
    xLABEL      (label_cache)
    xJZ32       (0)
#endif

    /*
     * we really missed - load vertex
     */
    xMOV_rm_r   (rmSIB,rEAX) xSIBID(rEBX,x4) xOFS32 (mMEM32(global.dwVertexCache)) // mov [vc + ebx*4],eax
}

/*
 * ILCCompile_loadVertex
 *
 * loads a vertex unless it is already cached
 */
void ILCCompile_loadVertex
(
    PNVD3DCONTEXT pContext,
    DWORD vertex,
    DWORD vertexAddress,
    DWORD indexAddress,
    BOOL  bAllowVertexCache,
    DWORD dwArbitrationLevel    // 0 - none, 1 - check v0, 2 - check v0 & v1
)
{
    // setup cache variables
    DWORD cacheSize = (ilcFlags & FLAG_DX6) ? 7 : 15;
    DWORD arbAdd    = (ilcFlags & FLAG_DX6) ? 3 : 7;

    DWORD l1,l2;

    // check cache
    if (bAllowVertexCache)
    {
        // find index and map directly on to cache
        xMOV_r_i32   (rEBX,indexAddress)
        xAND_rm_imm  (rmREG(rEBX),cacheSize)

        /*
         * arbitrate vertex
         *  at this point ebx will has the intended index
         *  at exit it will have a valid and usable index
         */
        switch (dwArbitrationLevel)
        {
            case 1:
            {
                DWORD l1,l2;

                xMOV_r_rm   (rEAX,rmREG(rEDI))
                xSHR_rm_imm8(rmREG(rEAX),8)
                xLABEL      (l1)
                xJMP        (0)
                 xLABEL      (l2)
                 xADD_rm_imm (rmREG(rEBX),arbAdd)
                 xAND_rm_imm (rmREG(rEBX),cacheSize)
                xTARGET_jmp (l1)
                xCMP_r_rm   (rEAX,rmREG(rEBX))
                xJZ         (l2)
                break;
            }
            case 2:
            {
                DWORD l1,l2;

                xMOV_r_rm   (rECX,rmREG(rEDI))
                xMOV_r_rm   (rEAX,rmREG(rEDI))
                xSHR_rm_imm8(rmREG(rECX),4)
                xSHR_rm_imm8(rmREG(rEAX),8)
                xAND_rm_imm (rmREG(rECX),cacheSize)
                xLABEL      (l1)
                xJMP        (0)
                 xLABEL      (l2)
                 xADD_rm_imm (rmREG(rEBX),arbAdd)
                 xAND_rm_imm (rmREG(rEBX),cacheSize)
                xTARGET_jmp (l1)
                xCMP_r_rm   (rEAX,rmREG(rEBX))
                xJZ         (l2)
                xCMP_r_rm   (rECX,rmREG(rEBX))
                xJZ         (l2)
                break;
            }
        }

        // jump over copy code to the vertex cache hit test
        xLABEL (l1)
        xJMP   (0)

        // cache miss - prepare to copy
        xLABEL (l2)
        ILCCompile_missCache();
        xPUSH_r (rEBX)
    }

    // copy vertex
    ILCCompile_vertex (pContext, vertex, vertexAddress, bAllowVertexCache ? ~0/*ebx*/ : vertex,
                       bAllowVertexCache, dwArbitrationLevel, FALSE);

    // more cache checking
    if (bAllowVertexCache)
    {
        DWORD l3;
        xPOP_r      (rECX) // HW vc location
        xLABEL      (l3)
        xJMP        (0)
        xTARGET_jmp (l1)
        ILCCompile_testCache (indexAddress,l2); // ebx has post-arb index
        if ((dwArbitrationLevel == 0)
         && (ilcFlags & MASK_FOG)
         && (ilcFlags & FLAG_KATMAI))
        {
            xPUSH_r (rECX)
            ILCCompile_KatmaiFog(FALSE);
            xPOP_r  (rECX)
        }
        xTARGET_jmp (l3)

        // update launch code
        xSHL_rm_imm8(rmREG(rECX),8)
        xSHR_rm_imm8(rmREG(rEDI),4)
        xOR_r_rm    (rEDI,rmREG(rECX))
    }
}

#ifndef NOCOMPRESSOVERHEAD

/*
 * ILCCompile_testInLine
 *
 * tests whether two vertices can be loaded together
 */
void ILCCompile_testInLine
(
    DWORD index0,
    DWORD index1
)
{
    DWORD and = (ilcFlags & FLAG_DX6) ? 7 : 15;

    xMOV_r_i32  (rEAX,index0)
     xMOV_r_i32  (rEBX,index1)
    xAND_rm_imm (rmREG(rEAX),and)
     xAND_rm_imm (rmREG(rEBX),and)
    xSUB_r_rm   (rEBX,rmREG(rEAX))
}

/*
 * ILCCompile_testCached
 *
 * test if the given index is cached and then tag is as loaded
 */
void ILCCompile_testCached
(
    DWORD index
)
{
    DWORD and = (ilcFlags & FLAG_DX6) ? 7 : 15;

    xMOV_r_i32  (rEAX,index)
    xMOV_r_rm   (rEBX,rmREG(rEAX))
    xAND_rm_imm (rmREG(rEAX),and)
    xOR_r_i32   (rEBX,mMEM32(global.dwVertexSN))
    xCMP_r_rm   (rEBX,rmSIB) xSIBID(rEAX,x4) xOFS32 (mMEM32(global.dwVertexCache)) // cmp ebx,[vc + eax*4]
    xMOV_rm_r   (rmSIB,rEBX) xSIBID(rEAX,x4) xOFS32 (mMEM32(global.dwVertexCache)) // mov [vc + eax*4],ebx
}

/*
 * ILCCompile_loadTriangle
 *
 * loads a whole indexed triangle. tries to be smart about load overhead
 * - leaves launch code in edi
 */
void ILCCompile_loadTriangle
(
    void
)
{
    DWORD l00,l01,l02,l03,l04,l05,l06,l07;
    DWORD l08,l09,l10,l11,l12,l13;
    DWORD l20,l21,l22,l23,l24;
    DWORD and = (ilcFlags & FLAG_DX6) ? 7 : 15;

    /*
     * main test tree
     */
    ILCCompile_testInLine (mMEM32(global.dwIndex0),mMEM32(global.dwIndex1));
    xLABEL (l20)
    xJZ32  (0)
    xDEC_rm (rmREG(rEBX))
    xLABEL (l00)
    xJNZ32 (0)

    ILCCompile_testInLine (mMEM32(global.dwIndex1),mMEM32(global.dwIndex2));
    xLABEL (l21)
    xJZ32  (0)
    xDEC_rm (rmREG(rEBX))
    xLABEL (l01)
    xJNZ32 (0)

    ILCCompile_testCached (mMEM32(global.dwIndex1));
    xLABEL (l03)
    xJZ32 (0)

    ILCCompile_testCached (mMEM32(global.dwIndex0));
    xLABEL (l02)
    xJZ32 (0)

    ILCCompile_testCached (mMEM32(global.dwIndex2));
    xLABEL (l04)
    xJZ32 (0)

    /*
     * 1,2,3
     */
    xMOV_r_i32  (rEBX,mMEM32(global.dwIndex0))
    xAND_rm_imm (rmREG(rEBX),and)
    xMOV_r_rm   (rEAX,rmREG(rEBX))
    xMOV_r_rm   (rEDI,rmREG(rEBX))
    xINC_rm     (rmREG(rEAX))
    xSHL_rm_imm8(rmREG(rEAX),4)
    xOR_r_rm    (rEDI,rmREG(rEAX))
    xADD_rm_imm (rmREG(rEAX),16)
    xSHL_rm_imm8(rmREG(rEAX),4)
    xOR_r_rm    (rEDI,rmREG(rEAX))
    ILCCompile_vertex (0,mMEM32(global.dwVertex0),0x80000003,TRUE,0,FALSE);
    ILCCompile_vertex (1,mMEM32(global.dwVertex1),0xfffffffe,TRUE,0,FALSE);
    ILCCompile_vertex (2,mMEM32(global.dwVertex2),0xfffffffe,TRUE,0,FALSE);
    xLABEL (l05)
    xJMP   (0)

    /*
     * second test tree
     */
    xTARGET_b32 (l00)

    ILCCompile_testInLine (mMEM32(global.dwIndex1),mMEM32(global.dwIndex2));
    xLABEL (l22)
    xJZ32  (0)
    xDEC_rm (rmREG(rEBX))
    xLABEL (l06)
    xJNZ32 (0)

    xTARGET_b32 (l02)

    ILCCompile_testCached (mMEM32(global.dwIndex1));
    xLABEL (l07)
    xJZ32 (0)

    ILCCompile_testCached (mMEM32(global.dwIndex2));
    xLABEL (l08)
    xJZ32 (0)

    /*
     * 1+2,3
     */
    xMOV_r_i32  (rEBX,mMEM32(global.dwIndex0))
    xAND_rm_imm (rmREG(rEBX),and)
    xMOV_r_rm   (rEDI,rmREG(rEBX))
    ILCCompile_vertex (0,mMEM32(global.dwVertex0),0x80000001,TRUE,0,FALSE);
    xMOV_r_i32  (rEBX,mMEM32(global.dwIndex1))
    xAND_rm_imm (rmREG(rEBX),and)
    xMOV_r_rm   (rEAX,rmREG(rEBX))
    xSHL_rm_imm8(rmREG(rEAX),4)
    xOR_r_rm    (rEDI,rmREG(rEAX))
    xADD_rm_imm (rmREG(rEAX),16)
    xSHL_rm_imm8(rmREG(rEAX),4)
    xOR_r_rm    (rEDI,rmREG(rEAX))
    ILCCompile_vertex (1,mMEM32(global.dwVertex1),0x80000002,TRUE,0,FALSE);
    ILCCompile_vertex (2,mMEM32(global.dwVertex2),0xfffffffe,TRUE,0,FALSE);
    xLABEL (l09)
    xJMP   (0)

    /*
     * third test tree
     */
    xTARGET_b32 (l01)

    // v0 and v2 can still content - test & handle properly
    ILCCompile_testInLine (mMEM32(global.dwIndex0),mMEM32(global.dwIndex2));
    xLABEL (l24)
    xJZ32  (0)

    ILCCompile_testCached (mMEM32(global.dwIndex1));
    xLABEL (l11)
    xJZ32 (0)

    ILCCompile_testCached (mMEM32(global.dwIndex0));
    xLABEL (l10)
    xJZ32 (0)

    xTARGET_b32 (l04)

    /*
     * 1,2+3
     */
    xMOV_r_i32  (rEBX,mMEM32(global.dwIndex0))
    xAND_rm_imm (rmREG(rEBX),and)
    xMOV_r_rm   (rEAX,rmREG(rEBX))
    xMOV_r_rm   (rEDI,rmREG(rEBX))
    xINC_rm     (rmREG(rEAX))
    xSHL_rm_imm8(rmREG(rEAX),4)
    xOR_r_rm    (rEDI,rmREG(rEAX))
    ILCCompile_vertex (0,mMEM32(global.dwVertex0),0x80000002,TRUE,0,FALSE);
    ILCCompile_vertex (1,mMEM32(global.dwVertex1),0xfffffffe,TRUE,0,FALSE);
    xMOV_r_i32  (rEBX,mMEM32(global.dwIndex2))
    xAND_rm_imm (rmREG(rEBX),and)
    xMOV_r_rm   (rEAX,rmREG(rEBX))
    xSHL_rm_imm8(rmREG(rEAX),8)
    xOR_r_rm    (rEDI,rmREG(rEAX))
    ILCCompile_vertex (2,mMEM32(global.dwVertex2),0x80000001,TRUE,0,FALSE);
    xLABEL (l12)
    xJMP   (0)

    /*
     * fourth test tree
     */
    xTARGET_b32 (l03)
    xTARGET_b32 (l06)
    xTARGET_b32 (l07)
    xTARGET_b32 (l08)
    xTARGET_b32 (l10)
    xTARGET_b32 (l11)

    // v0 and v2 can still content - test & handle properly
    ILCCompile_testInLine (mMEM32(global.dwIndex0),mMEM32(global.dwIndex2));
    xLABEL (l23)
    xJZ32  (0)

    /*
     * 1+2+3
     */
    xMOV_r_i32  (rEBX,mMEM32(global.dwIndex0))
    xAND_rm_imm (rmREG(rEBX),and)
    xMOV_r_rm   (rEDI,rmREG(rEBX))
    ILCCompile_vertex (0,mMEM32(global.dwVertex0),0x80000001,TRUE,0,FALSE);
    xMOV_r_i32  (rEBX,mMEM32(global.dwIndex1))
    xAND_rm_imm (rmREG(rEBX),and)
    xMOV_r_rm   (rEAX,rmREG(rEBX))
    xSHL_rm_imm8(rmREG(rEAX),4)
    xOR_r_rm    (rEDI,rmREG(rEAX))
    ILCCompile_vertex (1,mMEM32(global.dwVertex1),0x80000001,TRUE,0,FALSE);
    xMOV_r_i32  (rEBX,mMEM32(global.dwIndex2))
    xAND_rm_imm (rmREG(rEBX),and)
    xMOV_r_rm   (rEAX,rmREG(rEBX))
    xSHL_rm_imm8(rmREG(rEAX),8)
    xOR_r_rm    (rEDI,rmREG(rEAX))
    ILCCompile_vertex (2,mMEM32(global.dwVertex2),0x80000001,TRUE,0,FALSE);
    xLABEL (l13)
    xJMP   (0)

    /*
     * 1+2+3 with arbitration
     */
    xTARGET_b32 (l20)
    xTARGET_b32 (l21)
    xTARGET_b32 (l22)
    xTARGET_b32 (l23)
    xTARGET_b32 (l24)

    ILCCompile_loadVertex (0,mMEM32(global.dwVertex0),mMEM32(global.dwIndex0),TRUE,0);
    ILCCompile_loadVertex (1,mMEM32(global.dwVertex1),mMEM32(global.dwIndex1),TRUE,1);
    ILCCompile_loadVertex (2,mMEM32(global.dwVertex2),mMEM32(global.dwIndex2),TRUE,2);

    /*
     * done
     */
    xTARGET_jmp (l05)
    xTARGET_jmp (l09)
    xTARGET_jmp (l12)
    xTARGET_jmp (l13)
}

#endif // !NOCOMPRESSOVERHEAD

/*
 * ILCCompile_getFlatComponents
 *
 * load the appropriate flat shaded components. uses eax and ecx
 */
void ILCCompile_getFlatComponents
(
    PNVD3DCONTEXT pContext
)
{
    /*
     * flat shading always takes the 1st vertex unless it is a
     *  fan which takes the second
     */
    DWORD addr = (ISFAN(ilcFlags)) ? mMEM32(global.dwVertex1)
                                   : mMEM32(global.dwVertex0);

    if (pContext->pCurrentVShader->getFVF() & D3DFVF_DIFFUSE)
    {
        /*
         * load address
         */
        if (ISSTRIP(ilcFlags))
        {
            xMOV_r_i32  (rEAX,mMEM32(global.dwStripSense))
            xMOV_r_rm   (rEAX,rmIND32(rEAX)) xOFS32(addr)
        }
        else
        {
            xMOV_r_i32  (rEAX,addr)
        }
        /*
         * prepare color
         */
        xMOV_r_rm   (rECX,rmIND8(rEAX)) xOFS8(pContext->pCurrentVShader->getVAOffset(defaultInputRegMap[D3DVSDE_DIFFUSE]))
        xMOV_i32_r  (mMEM32(global.dwFlatColor),rECX)
    }
    if (pContext->pCurrentVShader->getFVF() & D3DFVF_SPECULAR)
    {
        /*
         * load address if we do not have it yet
         */
        if (!(pContext->pCurrentVShader->getFVF() & D3DFVF_DIFFUSE))
        {
            if (ISSTRIP(ilcFlags))
            {
                xMOV_r_i32  (rEAX,mMEM32(global.dwStripSense))
                xMOV_r_rm   (rEAX,rmIND32(rEAX)) xOFS32(addr)
            }
            else
            {
                xMOV_r_i32  (rEAX,addr)
            }
        }
        /*
         * prepare specular
         */
        xMOV_r_rm   (rECX,rmIND8(rEAX)) xOFS8(pContext->pCurrentVShader->getVAOffset(defaultInputRegMap[D3DVSDE_SPECULAR]))
        xAND_rm_imm (rmREG(rECX),0x00ffffff)
        xMOV_i32_r  (mMEM32(global.dwFlatSpecular),rECX)
    }
}


/*
 * ILCCompile_specularCheck
 *
 * checks if we can disable HW specular if primitives do not use it
 */
void ILCCompile_specularCheck
(
    PNVD3DCONTEXT pContext
)
{
    BOOL  bAlwaysOff = !(pContext->pCurrentVShader->getFVF() & D3DFVF_SPECULAR);
    DWORD l1,l2,l3;
#ifdef SPECULARPERFORMANCEFIX
    DWORD l4;
#endif

    /*
     * get specular components
     */
    if (!bAlwaysOff)
    {
        xMOV_r_i32   (rEAX,mMEM32(global.dwVertex0))
        xMOV_r_i32   (rEBX,mMEM32(global.dwVertex1))
        xMOV_r_i32   (rECX,mMEM32(global.dwVertex2))
        xMOV_r_rm    (rEAX,rmIND8(rEAX)) xOFS8 (pContext->pCurrentVShader->getVAOffset(defaultInputRegMap[D3DVSDE_SPECULAR]))
        xOR_r_rm     (rEAX,rmIND8(rEBX)) xOFS8 (pContext->pCurrentVShader->getVAOffset(defaultInputRegMap[D3DVSDE_SPECULAR]))
        xOR_r_rm     (rEAX,rmIND8(rECX)) xOFS8 (pContext->pCurrentVShader->getVAOffset(defaultInputRegMap[D3DVSDE_SPECULAR]))
        xTEST_rm_imm (rmREG(rEAX),0x00fcfcfc)
        xLABEL       (l1)
        xJZ          (0)    // no specular

#ifdef SPECULARPERFORMANCEFIX
        //short circuit the setting of specular on if it is already on
        xMOV_r_i32   (rEAX,mMEM32(global.dwSpecularState))
        xTEST_rm_imm (rmREG(rEAX),0x00000001)
        xLABEL       (l4)
        xJNZ         (0)
        /*
         * turn specular on & reset threshold
         */
        xMOV_r_i32   (rEAX,mMEM32(global.nvCachedPut))
        xMOV_r_i32   (rEBX,mMEM32(global.dwBlendRegister))
        xMOV_rm_imm  (rmREG(rECX),((1<<2)<<16) | (NV_DD_SPARE<<13) | NV054_BLEND)
        xMOV_rm_r    (rmIND(rEAX),rECX)
        xLEA_r_rm    (rEAX,rmIND8(rEAX)) xOFS8(8)
        xMOV_rm_r    (rmIND8(rEAX),rEBX) xOFS8(-4)
        xMOV_i32_r   (mMEM32(global.nvCachedPut),rEAX)
        xMOV_rm_imm  (rmREG(rEBX),1)
        xMOV_i32_r   (mMEM32(global.dwSpecularState),rEBX)
        xTARGET_b8   (l4)
        xMOV_rm_imm  (rmREG(rEBX),5)
        xMOV_i32_r   (mMEM32(global.dwSpecularCount),rEBX)

        xLABEL       (l2)
        xJMP         (0)
#else
        xMOV_r_i32   (rEAX,mMEM32(global.nvCachedPut))
        xMOV_r_i32   (rEBX,mMEM32(global.dwBlendRegister))
        xMOV_rm_imm  (rmREG(rECX),((1<<2)<<16) | (NV_DD_SPARE<<13) | NV054_BLEND)
        xMOV_rm_r    (rmIND(rEAX),rECX)
        xLEA_r_rm    (rEAX,rmIND8(rEAX)) xOFS8(8)
        xMOV_rm_r    (rmIND8(rEAX),rEBX) xOFS8(-4)
        xMOV_i32_r   (mMEM32(global.nvCachedPut),rEAX)
        xMOV_rm_imm  (rmREG(rEBX),5)
        xMOV_i32_r   (mMEM32(global.dwSpecularCount),rEBX)

        xLABEL       (l2)
        xJMP         (0)
#endif
        /*
         * decrement threshold
         */
        xTARGET_b8 (l1)
        xMOV_r_i32 (rEAX,mMEM32(global.dwSpecularCount))
        xDEC_rm    (rmREG(rEAX))
        xMOV_i32_r (mMEM32(global.dwSpecularCount),rEAX)
        xLABEL     (l3)
        xJNZ       (0)
    }

    /*
     * turn specular off
     */
    xMOV_r_i32   (rEAX,mMEM32(global.nvCachedPut))
    xMOV_r_i32   (rEBX,mMEM32(global.dwBlendRegister))
    xMOV_rm_imm  (rmREG(rECX),((1<<2)<<16) | (NV_DD_SPARE<<13) | NV054_BLEND)
    xAND_rm_imm  (rmREG(rEBX),0xffff0fff)
    xMOV_rm_r    (rmIND(rEAX),rECX)
    xLEA_r_rm    (rEAX,rmIND8(rEAX)) xOFS8(8)
    xMOV_rm_r    (rmIND8(rEAX),rEBX) xOFS8(-4)
    xMOV_i32_r   (mMEM32(global.nvCachedPut),rEAX)
#ifdef SPECULARPERFORMANCEFIX
    xMOV_rm_imm  (rmREG(rEBX),0)
    xMOV_i32_r   (mMEM32(global.dwSpecularState),rEBX)
#endif

    if (!bAlwaysOff)
    {
        xTARGET_b8  (l3)
        xTARGET_jmp (l2)
    }
}

/*
 * ILCCompile_cullcheck
 *
 * do a cull check on the triangle loaded in dwVertex0..2
 * code ends with the z flag set to indicate culled (do not draw)
 */
void ILCCompile_cullcheck
(
    void
)
{
    /*
     * compute cross product
     */
    if (pDriverData->nvD3DPerfData.dwCPUFeatureSet & FS_KATMAI)
    {
        /*
         * katmai
         */
        xMOV_r_i32       (rEAX,mMEM32(global.dwVertex0))
        xMOV_r_i32       (rEBX,mMEM32(global.dwVertex1))
        xMOV_r_i32       (rECX,mMEM32(global.dwVertex2))

        xMOVLPS_r_rm     (rXMM0,rmIND(rEAX))
        xMOVLPS_r_rm     (rXMM1,rmIND(rEBX))
        xSHUFPS_r_rm_imm (rXMM0,rmREG(rXMM0),0x44)          // xmm0: y0     x0      y0      x0
        xMOVHPS_r_rm     (rXMM1,rmIND(rECX))                // xmm1: y2     x2      y1      x1

        xSUBPS_r_rm      (rXMM1,rmREG(rXMM0))               // xmm1: y2-y0  x2-x0   y1-y0   x1-x0
        xSHUFPS_r_rm_imm (rXMM1,rmREG(rXMM1),0xe1)          // xmm1: y2-y0  x2-x0   x1-x0   y1-y0

        xMOVHLPS_r_r     (rXMM0,rXMM1)                      // xmm0: ?      ?       y2-y0   x2-x0
        xMULPS_r_rm      (rXMM0,rmREG(rXMM1))               // xmm0: ?      ?       (x1-x0)*(y2-y0) (y1-y0)*(x2-x0)

        xMOVAPS_r_rm     (rXMM1,rmREG(rXMM0))
        xSHUFPS_r_rm_imm (rXMM1,rmREG(rXMM1),0xe1)          // xmm1: ?      ?       ?       (x1-x0)*(y2-y0)

        xSUBSS_r_rm      (rXMM1,rmREG(rXMM0))               // xmm1: ?      ?       ?       (x1-x0)*(y2-y0)-(y1-y0)*(x2-x0)
        xMOVSS_i32_r     (mMEM32(global.dwTemp),rXMM1)
        xMOV_r_i32       (rEAX,mMEM32(global.dwTemp))
        xXOR_r_i32       (rEAX,mMEM32(global.dwCullMask1))
        xTEST_r_i32      (rEAX,mMEM32(global.dwCullMask2))
    }
    else
#if 0
// Not ready yet -- NickT
    if (global.dwCPUFeatureSet & FS_3DNOW)
    {
        /*
         * 3dnow
         */
        xMOV_r_i32(rEAX,mMEM32(global.dwVertex0))
        xMOV_r_i32(rEBX,mMEM32(global.dwVertex1))
        xMOV_r_i32(rECX,mMEM32(global.dwVertex2))

        xFEMMS                                      // Switch to 3dnow mode

        xMOVQ_r_rm(rMM0,rmIND(rEAX))                // mm0 =             Y0|X0
        xMOVQ_r_rm(rMM1,rmIND(rEBX))                // mm1 =             Y1|X1
        xMOVQ_r_rm(rMM2,rmIND(rECX))                // mm2 =             Y2|X2

        xPFSUB_r_rm(rMM1,rmREG(rMM0))               // mm1 =          Y1-Y0|X1-X0
        xPFSUB_r_rm(rMM2,rmREG(rMM0))               // mm2 =          Y2-Y0|X1-X0
        xMOVQ_r_rm(rMM0,rmREG(rMM1))                // mm0 =          Y1-Y0|X1-X0

        xPUNPCKLDQ_r_rm(rMM1,rmREG(rMM1))           // mm1 =          X1-X0|X1-Y0
        xPUNPCKHDQ_r_rm(rMM0,rmREG(rMM1))           // mm0 =          X1-X0|Y1-Y0

        xPFMUL_r_rm(rMM0,rmREG(rMM2))               // mm0 = (X1-X0)(Y2-Y0)|(Y1-Y0)(X2-X0)
        xMOVQ_r_rm(rMM1,rmREG(rMM0))                // mm1 = (X1-X0)(Y2-Y0)|(Y1-Y0)(X2-X0)

        xPUNPCKHDQ_r_rm(rMM1,rmREG(rMM1))           // mm1 =           xxxx|(X1-X0)(Y2-Y0)
        xPFSUB_r_rm(rMM1,rmREG(rMM0))               // mm1 =           xxxx|(X1-X0)(Y2-Y0)-(Y1-Y0)(X2-X0)

        xMOVD_r_rm(rEAX,rmREG(rMM1))                // EAX = (X1-X0)(Y2-Y0)-(Y1-Y0)(X2-X0)

        xFEMMS                                      // Switch out of 3dnow mode

        xXOR_r_i32 (rEAX,mMEM32(global.dwCullMask1))
        xTEST_r_i32(rEAX,mMEM32(global.dwCullMask2))
    }
    else
#endif
    {
        /*
         * generic
         */
        xMOV_r_i32 (rEAX,mMEM32(global.dwVertex0))  //
        xFLD_rm32  (rmIND(rEAX))                    // X0
        xMOV_r_i32 (rEBX,mMEM32(global.dwVertex1))  //
        xFLD_rm32  (rmIND(rEBX))                    // X1 | X0
        xFXCH_st   (rST1)                           // X0 | X1
        xFSUBP_st  (rST1)                           // X1-X0
        xMOV_r_i32 (rECX,mMEM32(global.dwVertex2))  //
        xFLD_rm32  (rmIND8(rECX)) xOFS8(4)          // Y2 | X1-X0
        xFXCH_st   (rST1)                           // X1-X0 | Y2
        xFLD_rm32  (rmIND8(rEAX)) xOFS8(4)          // Y0 | X1-X0 | Y2
        xFXCH_st   (rST1)                           // X1-X0 | Y0 | Y2
        xFLD_rm32  (rmIND(rECX))                    // X2 | X1-X0 | Y0 | Y2
        xFXCH_st   (rST3)                           // Y2 | X1-X0 | Y0 | X2
        xFLD_rm32  (rmIND(rEAX))                    // X0 | Y2 | X1-X0 | Y0 | X2
        xFXCH_st   (rST3)                           // Y0 | Y2 | X1-X0 | X0 | X2
        xFSUBP_st  (rST1)                           // Y2-Y0 | X1-X0 | X0 | X2
        xFXCH_st   (rST3)                           // X2 | X1-X0 | X0 | Y2-Y0
        xFLD_rm32  (rmIND8(rEBX)) xOFS8(4)          // Y1 | X2 | X1-X0 | X0 | Y2-Y0
        xFXCH_st   (rST3)                           // X0 | X2 | X1-X0 | Y1 | Y2-Y0
        xFSUBP_st  (rST1)                           // X2-X0 | X1-X0 | Y1 | Y2-Y0
        xFXCH_st   (rST3)                           // Y2-Y0 | X1-X0 | Y1 | X2-X0
        xFLD_rm32  (rmIND8(rEAX)) xOFS8(4)          // Y0 | Y2-Y0 | X1-X0 | Y1 | X2-X0
        xFXCH_st   (rST2)                           // X1-X0 | Y2-Y0 | Y0 | Y1 | X2-X0
        xFMULP_st  (rST1)                           // (X1-X0)(Y2-Y0) | Y0 | Y1 | X2-X0
        xFXCH_st   (rST1)                           // Y0 | (X1-X0)(Y2-Y0) | Y1 | X2-X0
        xFSUBP_st  (rST2)                           // (X1-X0)(Y2-Y0) | Y1-Y0 | X2-X0
        xFXCH_st   (rST2)                           // X2-X0 | Y1-Y0 | (X1-X0)(Y2-Y0)
        xFMULP_st  (rST1)                           // (X2-X0)(Y1-Y0) | (X1-X0)(Y2-Y0)
        xFSUBP_st  (rST1)                           // (X1-X0)(Y2-Y0)-(X2-X0)(Y1-Y0)
        xFSTP_i32  (mMEM32(global.dwTemp))
        xMOV_r_i32 (rEAX,mMEM32(global.dwTemp))
        xXOR_r_i32 (rEAX,mMEM32(global.dwCullMask1))
        xTEST_r_i32(rEAX,mMEM32(global.dwCullMask2))
    }
}

void ILCCompile_GetSpace (void)
{
#ifndef NV_NULL_HW_DRIVER
    getDC()->nvPusher.setPut (global.nvCachedPut);
#endif
    nvPusherAdjust (0);
    global.nvCachedPut       = getDC()->nvPusher.getPut();
    global.nvCachedThreshold = getDC()->nvPusher.getThreshold();
}


/*
 * ILCCompileCode
 *
 * Returns the entry point of a loop that will emit triangles in the format
 * specified. fvfData is implicitly passed
 */
DWORD ILCCompileCode
(
    PNVD3DCONTEXT pContext,
    DWORD dwFlags
)
{
    DWORD label_entry;
    DWORD label_outerLoop;
    DWORD label_innerLoop;
    DWORD label_reserveSpace1;
    DWORD label_reserveSpace2;
    DWORD label_reserveSpace3;
    DWORD label_tri;
    DWORD label_quad;
    DWORD label_exit;
#ifndef NOSKIP
    DWORD label_skipFetch;
#endif
    DWORD label_culled;

#ifdef NOVERTEXCACHE
    BOOL  bAllowVertexCache = FALSE;
#else
    BOOL  bAllowVertexCache = (dwFlags & FLAG_FLAT) ? FALSE : TRUE;
#endif

    /*
     * setup
     */
    ilcFlags = dwFlags;

    /*------------------------------------------------------------------------
     * entry
     *-----------------------------------------------------------------------*/

    /*
     * align entry point
     */
    while (ilcCount & 31) { xINT3 }
    xLABEL (label_entry)

/*
if (ilcFlags == 0x30001018) { xINT3 }
/**/

    /*
     * save important registers
     */
    xPUSH_r (rEBX)
    xPUSH_r (rESI)
    xPUSH_r (rEDI)
    xPUSH_r (rEBP)

    /*
     * break into special cases
     */
#ifndef NOSPECIALCASE
    xMOV_r_i32  (rEDI,mMEM32(global.dwPrimCount))
    xCMP_rm_imm (rmREG(rEDI),1)
    xLABEL      (label_tri)
    xJZ32       (0)
    if (!ISLIST(ilcFlags) && !(ilcFlags & FLAG_FLAT))
    {
        xCMP_rm_imm (rmREG(rEDI),2)
        xLABEL      (label_quad)
        xJZ32       (0)
    }
#endif

    /*
     * zero out some values
     */
    xXOR_r_rm  (rEAX,rmREG(rEAX))
    xMOV_i32_r (mMEM32(global.dwStripSense),rEAX)

    /*
     * invalidate vertex cache
     */
    xMOV_r_i32 (rEAX,(DWORD)&global.dwVertexSN)
    xADD_rm_imm(rmREG(rEAX),0x10000)
    xMOV_i32_r ((DWORD)&global.dwVertexSN,rEAX)

    /*
     * setup outer loop
     */
    if (ISLIST(ilcFlags))
    {
        if (ilcFlags & FLAG_INDEXED)
        {
            /*
             * do nothing
             */
        }
        else
        {
            /*
             * setup first three vertices and indices
             */
            xMOV_r_i32  (rEAX,mMEM32(global.pVertices))
            xMOV_i32_r  (mMEM32(global.dwVertex0),rEAX)
            xADD_rm_imm (rmREG(rEAX),pContext->pCurrentVShader->getStride())
            xMOV_i32_r  (mMEM32(global.dwVertex1),rEAX)
            xADD_rm_imm (rmREG(rEAX),pContext->pCurrentVShader->getStride())
            xMOV_i32_r  (mMEM32(global.dwVertex2),rEAX)

            bAllowVertexCache = FALSE;
        }
    }
    else
    {
        if (ilcFlags & FLAG_INDEXED)
        {
            /*
             * setup first two indices and vertices
             */
            xMOV_r_i32  (rEAX,mMEM32(global.pIndices))
            xMOV_r_rm   (rEBX,rmIND(rEAX))
            xMOV_r_rm   (rECX,rmREG(rEBX))
            xSHR_rm_imm8(rmREG(rECX),16)
            xMOV_i32_r  (mMEM32(global.dwIndex1),rECX)
            ILCCompile_mul (rECX,pContext->pCurrentVShader->getStride());
#ifdef WINNT
            ILCCompile_checkValid (rECX);
#endif
            xMOV_r_i32  (rEDX,mMEM32(global.pVertices))
            xAND_rm_imm (rmREG(rEBX),0xffff)
            xMOV_i32_r (mMEM32(global.dwIndex0),rEBX)
            ILCCompile_mul (rEBX,pContext->pCurrentVShader->getStride());
#ifdef WINNT
            ILCCompile_checkValid (rEBX);
#endif
            xLEA_r_rm   (rEBX,rmSIB) xSIB(rEBX,rEDX,x1)
            xMOV_i32_r  (mMEM32(global.dwVertex0),rEBX)
            xLEA_r_rm   (rEAX,rmIND8(rEAX)) xOFS8(4)
            xLEA_r_rm   (rECX,rmSIB) xSIB(rECX,rEDX,x1)
            xMOV_i32_r  (mMEM32(global.dwVertex1),rECX)
            xMOV_i32_r  (mMEM32(global.pIndices),rEAX)
        }
        else
        {
            /*
             * setup all indices and vertices
             */
            xMOV_r_i32  (rEAX,mMEM32(global.pVertices))
            xMOV_i32_r  (mMEM32(global.dwVertex0),rEAX)
            xADD_rm_imm (rmREG(rEAX),pContext->pCurrentVShader->getStride())
            xMOV_i32_r  (mMEM32(global.dwVertex1),rEAX)
            xADD_rm_imm (rmREG(rEAX),pContext->pCurrentVShader->getStride())
            xMOV_i32_r  (mMEM32(global.dwVertex2),rEAX)

            xXOR_r_rm  (rEAX,rmREG(rEAX))
            xMOV_i32_r (mMEM32(global.dwIndex0),rEAX)
            xINC_rm    (rmREG(rEAX))
            xMOV_i32_r (mMEM32(global.dwIndex1),rEAX)
            xINC_rm    (rmREG(rEAX))
            xMOV_i32_r (mMEM32(global.dwIndex2),rEAX)
        }
    }

    /*------------------------------------------------------------------------
     * outer loop
     *-----------------------------------------------------------------------*/

    xMOV_r_i32  (rEAX,mMEM32(global.dwPrimCount))
    label_outerLoop = ilcCount;

    /*
     * figure out how many primitives to put in a batch
     *  - dwPrimBatchCount = min(n, dwPrimCount)
     *     with n = (prim is list) ? 32 : 96
     *  - dwPrimCount -= dwPrimBatchCount
     *  - if it is less than 8 prims, we do not bother prefetching
     *  - edi still has primcount from tri & quad check above
     */
#ifndef NOSKIP
    {
        DWORD skipAmount = 8;

        xMOV_i32_r  (mMEM32(global.dwPrimBatchCount),rEDI)
        xXOR_r_rm   (rEAX,rmREG(rEAX))
        xCMP_rm_imm (rmREG(rEDI),skipAmount)
        xSETLE_rm8  (rmREG(rAL))
        xDEC_rm     (rmREG(rEAX))
        xAND_r_i32  (rEAX,mMEM32(global.dwPrimCount))
        xCMP_rm_imm (rmREG(rEDI),skipAmount)
        xMOV_i32_r  (mMEM32(global.dwPrimCount),rEAX)
        xLABEL      (label_skipFetch)
        xJLE32      (0)
    }
#endif
    {
        DWORD n = ISLIST(ilcFlags) ? 32 : 96;

        xXOR_r_rm   (rEBX,rmREG(rEBX))
        xCMP_rm_imm (rmREG(rEAX),n)
        xLEA_r_rm   (rEAX,rmIND8(rEAX)) xOFS8(-(int)n)  // eax = count - n
        xSETG_rm8   (rmREG(rBL))                        // ebx = count > n ? 1 : 0
        xDEC_rm     (rmREG(rEBX))                       // ebx = count > n ? 0 : ~0
        xAND_r_rm   (rEBX,rmREG(rEAX))                  // ebx = count > n ? 0 : count - n
        xADD_rm_imm (rmREG(rEBX),n)                     // ebx = count > n ? n : count
        xMOV_i32_r  (mMEM32(global.dwPrimBatchCount),rEBX)
        xNEG_rm     (rmREG(rEBX))
        xLEA_r_rm   (rmREG(rEAX),rmSIB8) xSIB(rEAX,rEBX,x1) xOFS8(n)
        xMOV_i32_r  (mMEM32(global.dwPrimCount),rEAX)
    }

    /*------------------------------------------------------------------------
     * fetcher
     *-----------------------------------------------------------------------*/

    /*
     * compute # of vertices involved in batch
     *  - this is 3 * dwPrimBatchCount for normal lists,
     *    and dwPrimBatchCount for strip & fans
     *  - result is in edx
     */
    xMOV_r_i32 (rEDX,mMEM32(global.dwPrimBatchCount))
    if (ISLIST(ilcFlags))
    {
        xLEA_r_rm (rEDX,rmSIB) xSIB(rEDX,rEDX,x2)
    }

    /*
     * fetch into L1
     *  - fetch indices first if we have indexed primitives
     */
    if (ilcFlags & FLAG_INDEXED)
    {
        DWORD l1,l2;
        DWORD i,j;

        /*
         * touch indices
         */
        if (ISLEGACY(ilcFlags))
        {
            xMOV_r_i32  (rECX,mMEM32(global.dwPrimBatchCount))
            xSHL_rm_imm8(rmREG(rECX),2)
        }
        else
        {
            xLEA_r_rm (rECX,rmSIB) xSIB(rEDX,rEDX,x1)
        }
        xMOV_r_i32  (rEBX,mMEM32(global.pIndices))
        xLABEL      (l1)
         xCMP_rm_imm (rmREG(rECX),4)
         xLABEL      (l2)
         xJL         (0)
         xMOV_r_rm   (rEAX,rmIND(rEBX))
         xSUB_rm_imm (rmREG(rECX),32)
         xLEA_r_rm   (rEBX,rmIND8(rEBX)) xOFS8(32)
        xJNZ        (l1)
        xTARGET_b8  (l2)

        /*
         * touch vertices
         */
        xMOV_r_i32  (rEBX,mMEM32(global.pIndices))
        xMOV_r_i32  (rECX,mMEM32(global.pVertices))
        xLABEL      (l1)
         xXOR_r_rm   (rEAX,rmREG(rEAX))
    x16r xMOV_r_rm   (rAX,rmIND(rEBX))
         xLEA_r_rm   (rEBX,rmIND8(rEBX)) xOFS8(2)
        ILCCompile_mul (rEAX,pContext->pCurrentVShader->getStride());
#ifdef WINNT
         ILCCompile_checkValid (rEAX);
#endif
         xDEC_rm     (rmREG(rEDX))
         xMOV_r_rm   (rEBP,rmSIB) xSIB(rEAX,rECX,x1)
         i = pContext->pCurrentVShader->getStride();
         j = 32;
         while (i > 32)
         {
            xMOV_r_rm   (rEBP,rmSIB8) xSIB(rEAX,rECX,x1) xOFS8(j)
            j += 32;
            i -= 32;
         }
        xJNZ32 (l1)
    }
    else
    {
        DWORD l1,l2;

        /*
         * touch vertices
         */
        xMOV_r_i32  (rECX,mMEM32(global.pVertices))
        ILCCompile_mul (rEDX,pContext->pCurrentVShader->getStride());
#ifdef WINNT
        ILCCompile_checkValid (rEDX);
#endif
        xLABEL      (l1)
         xCMP_rm_imm (rmREG(rEDX),4)
         xLABEL      (l2)
         xJL         (0)
         xMOV_r_rm   (rEAX,rmIND(rECX))
         xSUB_rm_imm (rmREG(rEDX),32)
         xLEA_r_rm   (rECX,rmIND8(rECX)) xOFS8(32)
        xJNZ        (l1)
        xTARGET_b8  (l2)
    }

    /*------------------------------------------------------------------------
     * pusher
     *-----------------------------------------------------------------------*/
#ifndef NOSKIP
    xTARGET_b32 (label_skipFetch)
#endif

    /*
     * inner loop start
     */
    xLABEL (label_innerLoop)

    /*
     * use indices to generate vertex addresses
     */
    if (ISLIST(ilcFlags))
    {
        if (ilcFlags & FLAG_INDEXED)
        {
            /*
             * load indices and compute vertex addresses
             *  - read 3 indices and compute vertex = base + index * stride
             *  - increment pIndices
             */
            xMOV_r_i32  (rEBX,mMEM32(global.pIndices))
            xMOV_r_rm   (rECX,rmIND(rEBX))
            xMOV_r_rm   (rEDX,rmREG(rECX))
            xAND_rm_imm (rmREG(rECX),0xffff)
            xMOV_i32_r  (mMEM32(global.dwIndex0),rECX)
            xSHR_rm_imm8(rmREG(rEDX),16)
            xMOV_i32_r  (mMEM32(global.dwIndex1),rEDX)
            ILCCompile_mul (rECX,pContext->pCurrentVShader->getStride());
#ifdef WINNT
            ILCCompile_checkValid (rECX);
#endif
            xMOV_r_rm   (rEAX,rmIND8(rEBX)) xOFS8 (4)
            xLEA_r_rm   (rEBX,rmIND8(rEBX)) xOFS8 (ISLEGACY(ilcFlags) ? 8 : 6)
            xAND_rm_imm (rmREG(rEAX),0xffff)
            xMOV_i32_r  (mMEM32(global.pIndices),rEBX)
            ILCCompile_mul (rEDX,pContext->pCurrentVShader->getStride());
#ifdef WINNT
            ILCCompile_checkValid (rEDX);
#endif
            xMOV_i32_r  (mMEM32(global.dwIndex2),rEAX)
            ILCCompile_mul (rEAX,pContext->pCurrentVShader->getStride());
#ifdef WINNT
            ILCCompile_checkValid (rEAX);
#endif
            xMOV_r_i32  (rEBX,mMEM32(global.pVertices))
            xLEA_r_rm   (rECX,rmSIB) xSIB(rECX,rEBX,x1)
            xLEA_r_rm   (rEDX,rmSIB) xSIB(rEDX,rEBX,x1)
            xMOV_i32_r  (mMEM32(global.dwVertex0),rECX)
            xLEA_r_rm   (rEAX,rmSIB) xSIB(rEAX,rEBX,x1)
            xMOV_i32_r  (mMEM32(global.dwVertex1),rEDX)
            xMOV_i32_r  (mMEM32(global.dwVertex2),rEAX)
        }
        else
        {
            /*
             * do nothing
             */
        }
    }
    else
    {
        if (ilcFlags & FLAG_INDEXED)
        {
            /*
             * load index for third vertex
             */
            xXOR_r_rm   (rECX,rmREG(rECX))
            xMOV_r_i32  (rEBX,mMEM32(global.pIndices))
       x16r xMOV_r_rm   (rCX,rmIND(rEBX))
            xMOV_r_i32  (rEAX,mMEM32(global.pVertices))
            xLEA_r_rm   (rEBX,rmIND8(rEBX)) xOFS8 (2)
            xMOV_i32_r  (mMEM32(global.dwIndex2),rECX)
            ILCCompile_mul (rECX,pContext->pCurrentVShader->getStride());
#ifdef WINNT
            ILCCompile_checkValid (rECX);
#endif
            xMOV_i32_r  (mMEM32(global.pIndices),rEBX)
            xLEA_r_rm   (rECX,rmSIB) xSIB(rEAX,rECX,x1)
            xMOV_i32_r  (mMEM32(global.dwVertex2),rECX)
        }
        else
        {
            /*
             * do nothing
             */
        }
    }

    /*
     * reserve space in push buffer
     *  - always make space for 4 full dx6 triangles at a time
     *  - this will save on overhead when put chases get
     */
    {
        DWORD l1;

        xMOV_r_i32  (rEAX,mMEM32(global.nvCachedThreshold))
        xMOV_r_i32  (rECX,mMEM32(global.nvCachedPut))
        xMOV_r_i32  (rEBX,mMEM32(ilcData))
        xLABEL      (l1)
        xADD_rm_imm (rmREG(rEBX),0)
        xCMP_r_rm   (rECX,rmREG(rEAX))
        xLABEL      (label_reserveSpace1)
        xJGE32      (0)
        *(DWORD*)(ilcData + l1 + 2) = ilcCount;
    }

    /*
     * copy data to pushbuffer
     *  - we use the vertex cache if we can
     *  - edi will contain the launch codes
     */
    if (bAllowVertexCache)
    {
        xXOR_r_rm (rEDI,rmREG(rEDI))
    }

    /*
     * do cull check
     */
#ifndef NOCULLING
    ILCCompile_cullcheck();
    xLABEL (label_culled)
    xJZ32  (0)
#endif

    //
    // MUSH
    //

    if (pDriverData->nvD3DPerfData.dwPerformanceStrategy & PS_MUSH)
    {
        xFLD_i32  (mMEM32(global.dwTemp))
        xFADD_i32 (mMEM32(global.fTotalPixelsTimes2))
        xFSTP_i32 (mMEM32(global.fTotalPixelsTimes2))
    }

    /*
     * get flat shaded colors
     */
    if (ilcFlags & FLAG_FLAT)
    {
        ILCCompile_getFlatComponents (pContext);
    }

    /*
     * check specular
     */
    if (!(ilcFlags & FLAG_DX6) && (ilcFlags & FLAG_SPECULAR))
    {
        ILCCompile_specularCheck (pContext);
    }

    /*
     * copy
     */
#ifndef NOCOMPRESSOVERHEAD
    if (bAllowVertexCache)
    {
        ILCCompile_loadTriangle();
    }
    else
    {
#endif //!NOCOMPRESSOVERHEAD
        ILCCompile_loadVertex (pContext,0,mMEM32(global.dwVertex0),mMEM32(global.dwIndex0),bAllowVertexCache,0);
        ILCCompile_loadVertex (pContext,1,mMEM32(global.dwVertex1),mMEM32(global.dwIndex1),bAllowVertexCache,1);
        ILCCompile_loadVertex (pContext,2,mMEM32(global.dwVertex2),mMEM32(global.dwIndex2),bAllowVertexCache,2);
#ifndef NOCOMPRESSOVERHEAD
    }
#endif //!NOCOMPRESSOVERHEAD

    /*
     * launch triangle
     */
#ifndef NOLAUNCH
    xMOV_r_i32  (rEBX,mMEM32(global.nvCachedPut))
    if (!bAllowVertexCache)
    {
        xMOV_rm_imm (rmREG(rEDI),0x210)
    }
    xMOV_rm_imm (rmREG(rEAX),((ilcFlags & FLAG_DX6) ? 0x0004e540 : 0x0004e600))
    xMOV_rm_r   (rmIND(rEBX),rEAX)
    xLEA_r_rm   (rEBX,rmIND8(rEBX)) xOFS8(8)
    xMOV_rm_r   (rmIND8(rEBX),rEDI) xOFS8(-4)
    xMOV_i32_r  (mMEM32(global.nvCachedPut),rEBX)
#endif
#ifdef DEBUG
    xPUSHAD
    xMOV_rm_imm (rmREG(rEBX),(DWORD)ILCDebug_flushTriangle)
    xCALL_rm    (rmREG(rEBX))
    xPOPAD
#endif

#ifndef NOCULLING
    xTARGET_b32 (label_culled)
#endif

    /*
     * update indices and vertices for next primitive
     */
    if (ISLIST(ilcFlags))
    {
        if (ilcFlags & FLAG_INDEXED)
        {
            /*
             * do nothing
             */
        }
        else
        {
            /*
             * update vertices
             */
            xMOV_r_i32  (rEAX,mMEM32(global.dwVertex2))
            xADD_rm_imm (rmREG(rEAX),pContext->pCurrentVShader->getStride())
            xMOV_i32_r  (mMEM32(global.dwVertex0),rEAX)
            xADD_rm_imm (rmREG(rEAX),pContext->pCurrentVShader->getStride())
            xMOV_i32_r  (mMEM32(global.dwVertex1),rEAX)
            xADD_rm_imm (rmREG(rEAX),pContext->pCurrentVShader->getStride())
            xMOV_i32_r  (mMEM32(global.dwVertex2),rEAX)
        }
    }
    else
    {
        if (ilcFlags & FLAG_INDEXED)
        {
            if (ISSTRIP(ilcFlags))
            {
                /*
                 * update strip for culling sense
                 */
                xMOV_r_i32  (rEBX,mMEM32(global.dwStripSense))
                xMOV_r_i32  (rEAX,mMEM32(global.dwVertex2))
                xMOV_rm_r   (rmIND32(rEBX),rEAX) xOFS32(mMEM32(global.dwVertex0))

                xMOV_r_i32  (rEAX,mMEM32(global.dwIndex2))
                xMOV_rm_r   (rmIND32(rEBX),rEAX) xOFS32(mMEM32(global.dwIndex0))
                xXOR_rm_imm (rmREG(rEBX),4)
                xMOV_i32_r  (mMEM32(global.dwStripSense),rEBX)
            }
            else
            {
                /*
                 * update indices and vertices for fan
                 */
                xMOV_r_i32  (rEAX,mMEM32(global.dwVertex2))
                xMOV_i32_r  (mMEM32(global.dwVertex1),rEAX)

                xMOV_r_i32  (rEAX,mMEM32(global.dwIndex2))
                xMOV_i32_r  (mMEM32(global.dwIndex1),rEAX)
            }
        }
        else
        {
            if (ISSTRIP(ilcFlags))
            {
                /*
                 * update indices and vertices for strip
                 */
                xMOV_r_i32  (rEBX,mMEM32(global.dwStripSense))
                xMOV_r_i32  (rEAX,mMEM32(global.dwVertex2))
                xMOV_rm_r   (rmIND32(rEBX),rEAX) xOFS32(mMEM32(global.dwVertex0))
                xADD_rm_imm (rmREG(rEAX),pContext->pCurrentVShader->getStride())
                xMOV_i32_r  (mMEM32(global.dwVertex2),rEAX)

                xMOV_r_i32  (rEAX,mMEM32(global.dwIndex2))
                xMOV_rm_r   (rmIND32(rEBX),rEAX) xOFS32(mMEM32(global.dwIndex0))
                xINC_rm     (rmREG(rEAX))
                xXOR_rm_imm (rmREG(rEBX),4)
                xMOV_i32_r  (mMEM32(global.dwIndex2),rEAX)
                xMOV_i32_r  (mMEM32(global.dwStripSense),rEBX)
            }
            else
            {
                /*
                 * update indices and vertices for fan
                 */
                xMOV_r_i32  (rEAX,mMEM32(global.dwVertex2))
                xMOV_i32_r  (mMEM32(global.dwVertex1),rEAX)
                xADD_rm_imm (rmREG(rEAX),pContext->pCurrentVShader->getStride())
                xMOV_i32_r  (mMEM32(global.dwVertex2),rEAX)

                xMOV_r_i32  (rEAX,mMEM32(global.dwIndex2))
                xMOV_i32_r  (mMEM32(global.dwIndex1),rEAX)
                xINC_rm     (rmREG(rEAX))
                xMOV_i32_r  (mMEM32(global.dwIndex2),rEAX)
            }
        }
    }

    /*
     * loop back for next primitive
     */
    xMOV_r_i32 (rEAX,mMEM32(global.dwPrimBatchCount))
    xDEC_rm    (rmREG(rEAX))
    xMOV_i32_r (mMEM32(global.dwPrimBatchCount),rEAX)
    xJNZ32     (label_innerLoop)

    /*
     * loop back for next batch
     */
    xMOV_r_i32 (rEAX,mMEM32(global.dwPrimCount))
    xTEST_r_rm (rEAX,rmREG(rEAX))
    xJNZ32     (label_outerLoop) // eax has dwPrimCount loaded already

    /*------------------------------------------------------------------------
     * exit
     *-----------------------------------------------------------------------*/
    xLABEL     (label_exit)

    /*
     * reset HW specular state
     */
    if (!(ilcFlags & FLAG_DX6) && (ilcFlags & FLAG_SPECULAR))
    {
        /*
         * turn specular on
         */
        xMOV_r_i32   (rEAX,mMEM32(global.nvCachedPut))
        xMOV_r_i32   (rEBX,mMEM32(global.dwBlendRegister))
        xMOV_rm_imm  (rmREG(rECX),((1<<2)<<16) | (NV_DD_SPARE<<13) | NV054_BLEND)
        xMOV_rm_r    (rmIND(rEAX),rECX)
        xLEA_r_rm    (rEAX,rmIND8(rEAX)) xOFS8(8)
        xMOV_rm_r    (rmIND8(rEAX),rEBX) xOFS8(-4)
        xMOV_i32_r   (mMEM32(global.nvCachedPut),rEAX)
#ifdef SPECULARPERFORMANCEFIX
        xMOV_rm_imm  (rEAX,1)
        xMOV_i32_r   (mMEM32(global.dwSpecularState),rEAX)
#endif
    }

    /*
     * restore registers
     */
    xPOP_r (rEBP)
    xPOP_r (rEDI)
    xPOP_r (rESI)
    xPOP_r (rEBX)
    xRET

#ifndef NOSPECIALCASE
    /*------------------------------------------------------------------------
     * single triangle
     *-----------------------------------------------------------------------*/
    while (ilcCount & 31) { xINT3 }
    xTARGET_b32 (label_tri)

    /*
     * reserve space in push buffer
     *  - always make space for 4 full dx6 triangles at a time
     *  - this will save on overhead when put chases get
     */
    {
        DWORD l1;

        xMOV_r_i32  (rEAX,mMEM32(global.nvCachedThreshold))
        xMOV_r_i32  (rECX,mMEM32(global.nvCachedPut))
        xMOV_r_i32  (rEBX,mMEM32(ilcData))
        xLABEL      (l1)
        xADD_rm_imm (rmREG(rEBX),0)
        xCMP_r_rm   (rECX,rmREG(rEAX))
        xLABEL      (label_reserveSpace2)
        xJGE32      (0)
        *(DWORD*)(ilcData + l1 + 2) = ilcCount;
    }

    if (ilcFlags & FLAG_INDEXED)
    {
        /*
         * read indices and compute vertex offsets
         */
        xMOV_r_i32  (rEAX,mMEM32(global.pIndices))
        xMOV_r_rm   (rEBX,rmIND(rEAX))
        xMOV_r_rm   (rECX,rmREG(rEBX))
        xAND_rm_imm (rmREG(rEBX),0xffff)
        xSHR_rm_imm8(rmREG(rECX),16)
        xMOV_r_rm   (rEDX,rmIND8(rEAX)) xOFS8(4)
        ILCCompile_mul (rEBX,pContext->pCurrentVShader->getStride());
#ifdef WINNT
        ILCCompile_checkValid (rEBX);
#endif
        ILCCompile_mul (rECX,pContext->pCurrentVShader->getStride());
#ifdef WINNT
        ILCCompile_checkValid (rECX);
#endif
        xAND_rm_imm (rmREG(rEDX),0xffff)
        xMOV_r_i32  (rEAX,mMEM32(global.pVertices))
        ILCCompile_mul (rEDX,pContext->pCurrentVShader->getStride());
#ifdef WINNT
        ILCCompile_checkValid (rEDX);
#endif
        xLEA_r_rm   (rEBX,rmSIB) xSIB(rEBX,rEAX,x1)
        xMOV_i32_r  (mMEM32(global.dwVertex0),rEBX)
        xMOV_r_rm   (rEBX,rmIND(rEBX))
        xLEA_r_rm   (rECX,rmSIB) xSIB(rECX,rEAX,x1)
        xMOV_i32_r  (mMEM32(global.dwVertex1),rECX)
        xMOV_r_rm   (rEBX,rmIND(rECX))
        xLEA_r_rm   (rEDX,rmSIB) xSIB(rEDX,rEAX,x1)
        xMOV_i32_r  (mMEM32(global.dwVertex2),rEDX)
        xMOV_r_rm   (rEBX,rmIND(rEDX))
    }
    else
    {
        /*
         * compute vertex offsets
         */
        xMOV_r_i32  (rEAX,mMEM32(global.pVertices))
        xMOV_i32_r  (mMEM32(global.dwVertex0),rEAX)
        xMOV_r_rm   (rEBX,rmIND(rEAX))
        xADD_rm_imm (rmREG(rEAX),pContext->pCurrentVShader->getStride())
        xMOV_i32_r  (mMEM32(global.dwVertex1),rEAX)
        xMOV_r_rm   (rEBX,rmIND(rEAX))
        xADD_rm_imm (rmREG(rEAX),pContext->pCurrentVShader->getStride())
        xMOV_i32_r  (mMEM32(global.dwVertex2),rEAX)
        xMOV_r_rm   (rEBX,rmIND(rEAX))
    }
    /*
     * get flat shaded colors
     */
    if (ilcFlags & FLAG_FLAT)
    {
        ILCCompile_getFlatComponents (pContext);
    }
    /*
     * copy
     */
    ILCCompile_vertex (pContext,0,mMEM32(global.dwVertex0),0,FALSE,0/*edi = global.nvCachedPut*/,FALSE);
    ILCCompile_vertex (pContext,1,mMEM32(global.dwVertex1),1,FALSE,1,FALSE);
    ILCCompile_vertex (pContext,2,mMEM32(global.dwVertex2),2,FALSE,1,FALSE);
    /*
     * launch triangle, edi still has global.nvCachedPut
     */
#ifndef NOLAUNCH
    xMOV_rm_imm (rmREG(rEBX),0x210)
    xMOV_rm_imm (rmREG(rEAX),((ilcFlags & FLAG_DX6) ? 0x0004e540 : 0x0004e600))
    xMOV_rm_r   (rmIND(rEDI),rEAX)
    xLEA_r_rm   (rEDI,rmIND8(rEDI)) xOFS8(8)
    xMOV_rm_r   (rmIND8(rEDI),rEBX) xOFS8(-4)
    xMOV_i32_r  (mMEM32(global.nvCachedPut),rEDI)
#endif
#ifdef DEBUG
    xPUSHAD
    xMOV_rm_imm (rmREG(rEBX),(DWORD)ILCDebug_flushTriangle)
    xCALL_rm    (rmREG(rEBX))
    xPOPAD
#endif
    xJMP        (label_exit)

    /*------------------------------------------------------------------------
     * quad
     *-----------------------------------------------------------------------*/
    if (!ISLIST(ilcFlags) && !(ilcFlags & FLAG_FLAT))
    {
        while (ilcCount & 31) { xINT3 }
        xTARGET_b32 (label_quad)
        /*
         * reserve space in push buffer
         *  - always make space for 4 full dx6 triangles at a time
         *  - this will save on overhead when put chases get
         */
        {
            DWORD l1;

            xMOV_r_i32  (rEAX,mMEM32(global.nvCachedThreshold))
            xMOV_r_i32  (rECX,mMEM32(global.nvCachedPut))
            xMOV_r_i32  (rEBX,mMEM32(ilcData))
            xLABEL      (l1)
            xADD_rm_imm (rmREG(rEBX),0)
            xCMP_r_rm   (rECX,rmREG(rEAX))
            xLABEL      (label_reserveSpace3)
            xJGE32      (0)
            *(DWORD*)(ilcData + l1 + 2) = ilcCount;
        }

        /*
         * we compute the 4 vertices into dwVertex0, 1, 2 and pVertices
         */
        if (ilcFlags & FLAG_INDEXED)
        {
            /*
             * read indices and compute vertex offsets
             */
            xMOV_r_i32  (rEAX,mMEM32(global.pIndices))
            xMOV_r_rm   (rEBX,rmIND(rEAX))
            xMOV_r_rm   (rECX,rmREG(rEBX))
            xAND_rm_imm (rmREG(rEBX),0xffff)
            xSHR_rm_imm8(rmREG(rECX),16)
            xMOV_r_rm   (rEDX,rmIND8(rEAX)) xOFS8(4)
            ILCCompile_mul (rEBX,pContext->pCurrentVShader->getStride());
#ifdef WINNT
            ILCCompile_checkValid (rEBX);
#endif
            ILCCompile_mul (rECX,pContext->pCurrentVShader->getStride());
#ifdef WINNT
            ILCCompile_checkValid (rECX);
#endif
            xMOV_r_rm   (rEBP,rmREG(rEDX))
            xAND_rm_imm (rmREG(rEDX),0xffff)
            xMOV_r_i32  (rEAX,mMEM32(global.pVertices))
            ILCCompile_mul (rEDX,pContext->pCurrentVShader->getStride());
#ifdef WINNT
            ILCCompile_checkValid (rEDX);
#endif
            xSHR_rm_imm8(rmREG(rEBP),16)
            xLEA_r_rm   (rEBX,rmSIB) xSIB(rEBX,rEAX,x1)
            xMOV_i32_r  (mMEM32(global.dwVertex0),rEBX)
            xMOV_r_rm   (rEBX,rmIND(rEBX))
            xMOV_r_rm   (rEBX,rmREG(rEBP))
            xLEA_r_rm   (rECX,rmSIB) xSIB(rECX,rEAX,x1)
            xMOV_i32_r  (mMEM32(global.dwVertex1),rECX)
            xMOV_r_rm   (rECX,rmIND(rECX))
            ILCCompile_mul (rEBX,pContext->pCurrentVShader->getStride());
#ifdef WINNT
            ILCCompile_checkValid (rEBX);
#endif
            xLEA_r_rm   (rEDX,rmSIB) xSIB(rEDX,rEAX,x1)
            xMOV_i32_r  (mMEM32(global.dwVertex2),rEDX)
            xMOV_r_rm   (rECX,rmIND(rEDX))
            xLEA_r_rm   (rEBX,rmSIB) xSIB(rEBX,rEAX,x1)
            xMOV_i32_r  (mMEM32(global.pVertices),rEBX)
            xMOV_r_rm   (rECX,rmIND(rEBX))
        }
        else
        {
            /*
             * compute vertex offsets
             */
            xMOV_r_i32  (rEAX,mMEM32(global.pVertices))
            xMOV_i32_r  (mMEM32(global.dwVertex0),rEAX)
            xMOV_r_rm   (rEBX,rmIND(rEAX))
            xADD_rm_imm (rmREG(rEAX),pContext->pCurrentVShader->getStride())
            xMOV_i32_r  (mMEM32(global.dwVertex1),rEAX)
            xMOV_r_rm   (rEBX,rmIND(rEAX))
            xADD_rm_imm (rmREG(rEAX),pContext->pCurrentVShader->getStride())
            xMOV_i32_r  (mMEM32(global.dwVertex2),rEAX)
            xMOV_r_rm   (rEBX,rmIND(rEAX))
            xADD_rm_imm (rmREG(rEAX),pContext->pCurrentVShader->getStride())
            xMOV_i32_r  (mMEM32(global.pVertices),rEAX)
            xMOV_r_rm   (rEBX,rmIND(rEAX))
        }

        /*
         * copy
         */
        ILCCompile_vertex (pContext,0,mMEM32(global.dwVertex0),0,FALSE,0/*edi = global.nvCachedPut*/,TRUE);
        ILCCompile_vertex (pContext,1,mMEM32(global.dwVertex1),1,FALSE,1,TRUE);
        ILCCompile_vertex (pContext,2,mMEM32(global.dwVertex2),2,FALSE,1,TRUE);
        ILCCompile_vertex (pContext,3,mMEM32(global.pVertices),3,FALSE,1,TRUE);
        /*
         * launch triangle, edi still has global.nvCachedPut
         */
#ifndef NOLAUNCH
        xMOV_rm_imm (rmREG(rEBX),(ISSTRIP(ilcFlags) ? 0x312210 : 0x320210))
        xMOV_rm_imm (rmREG(rEAX),((ilcFlags & FLAG_DX6) ? 0x0004e540 : 0x0004e600))
        xMOV_rm_r   (rmIND(rEDI),rEAX)
        xLEA_r_rm   (rEDI,rmIND8(rEDI)) xOFS8(8)
        xMOV_rm_r   (rmIND8(rEDI),rEBX) xOFS8(-4)
        xMOV_i32_r  (mMEM32(global.nvCachedPut),rEDI)
#endif
#ifdef DEBUG
        xPUSHAD
        xMOV_rm_imm (rmREG(rEBX),(DWORD)ILCDebug_flushTriangle)
        xCALL_rm    (rmREG(rEBX))
        xPOPAD
#endif
        xJMP        (label_exit)
    }
#endif //!NOSPECIALCASE

    /*------------------------------------------------------------------------
     * reserveSpace
     *-----------------------------------------------------------------------*/
    /*
     * align entry point
     */
    while (ilcCount & 31) { xINT3 }
    xTARGET_b32 (label_reserveSpace1)
#ifndef NOSPECIALCASE
    xTARGET_b32 (label_reserveSpace2)
    if (!ISLIST(ilcFlags) && !(ilcFlags & FLAG_FLAT))
    {
        xTARGET_b32 (label_reserveSpace3)
    }
#endif
    {
        xPUSH_r    (rEBX) // return address
        xMOV_rm_imm(rmREG(rEBX),(DWORD)ILCCompile_GetSpace)
        xCALL_rm   (rmREG(rEBX)) // ebx, edi and esi should be preserved if compiler follow its own rules
        xRET
    }

    /*
     * done
     */
    return label_entry;
}


/****************************************************************************
 * C helper code                                                            *
 ****************************************************************************/

/*
 * TriangleSetup only choose the major things like:
 *      dx5 or dx6
 *      fog
 *      basic, mmx or katmai
 *      fixed wbuffer
 *
 * TriangleDispatch deal with:
 *      indexed vs non-indexed
 *      fvf vs non fvf
 *      list,strip,fan
 */

/*
 * ILCGrowTo
 *
 * grows ilcData to asked for (or larger) size
 */
void ILCGrowTo
(
    DWORD dwNeed
)
{
#ifdef VTUNE_SUPPORT
    if (!ilcMax)
    {
        nvVTuneModifyAccessRights();
        ilcData = (BYTE*)((DWORD)nvVTuneMemoryImage + 36);
        ilcMax =  *(DWORD*)(ilcData - 4);
    }
    else if (dwNeed > ilcMax)
    {
        DPF ("*** maximum block size overrun. adjust size ***");
        __asm int 3;
    }
#else //!VTUNE_SUPPORT
    /*
     * determine how much memory we need
     */
    if (dwNeed > ilcMax)
    {
        /*
         * grow
         */
#ifdef DEBUG
        DWORD ilcOld = ilcMax;
#endif
        ilcMax  = (dwNeed + 4095) & ~4095;
        // if this fails we are truly screwed
        void* pNewILC;
        if (!ReallocIPM(ilcData, ilcMax, &pNewILC))
        {
            DPF("ILCGrowTo: ReallocIPM failed - we are truly screwed");
        }
        else
        {
            ilcData = (BYTE*)pNewILC;
        }
#ifdef DEBUG
        if (!ilcData)
        {
            DPF ("Memory allocation failed - things are going to go terribly wrong now");
            _asm int 3
        }

        memset (ilcData+ilcOld,0xcc,ilcMax-ilcOld);
#endif
    }
#endif //!VTUNE_SUPPORT
}

/*
 * nvDX5TriangleSetup
 *
 * DX5 triangle setup
 */
void __stdcall nvDX5TriangleSetup
(
    PNVD3DCONTEXT pContext,
    DWORD  dwPrimCount,
    LPWORD pIndices,
    LPBYTE pVertices
)
{
    /*
     * if we have state changes, compute HW state now
     */
    if (pContext->bStateChange)
    {
        nvSetHardwareState (pContext);
    }

    /*
     * Send the context state down to the hardware.
     */
    nvSetDx5TriangleState(pContext);

    pContext->dwFunctionLookup = NV_FOG_MODE
                               | ((pContext->dwRenderState[D3DRENDERSTATE_SHADEMODE] == D3DSHADE_FLAT) ? FLAG_FLAT : 0)
 #ifndef FLOAT_W
                               | ((pContext->dwRenderState[D3DRENDERSTATE_ZENABLE] == D3DZB_USEW)      ? FLAG_FIXEDWBUF : 0)
 #endif
                               | ((pContext->dwRenderState[D3DRENDERSTATE_SPECULARENABLE] == TRUE)     ? FLAG_SPECULAR : 0)
                               | ((pDriverData->nvD3DPerfData.dwCPUFeatureSet & FS_MMX)                ? FLAG_MMX    : 0)
                               | ((pDriverData->nvD3DPerfData.dwCPUFeatureSet & FS_KATMAI)             ? FLAG_KATMAI : 0);

    if (pContext->pZetaBuffer)
    {
        CNvObject *pNvObj = pContext->pZetaBuffer->getWrapper();
        if (pNvObj->hasCTEnabled())
        {
            pContext->dwFunctionLookup |= pNvObj->isCTFront() ? FLAG_ZFP
                                                              : FLAG_ZBP;
        }
    }

    return;
}

/*
 * nvDX6TriangleSetup
 *
 * DX6 triangle setup
 */
void __stdcall nvDX6TriangleSetup
(
    PNVD3DCONTEXT pContext,
    DWORD  dwPrimCount,
    LPWORD pIndices,
    LPBYTE pVertices
)
{
    /*
     * setup HW if needed
     */
    if (pContext->bStateChange)
    {
        nvSetMultiTextureHardwareState (pContext);
        if (!pContext->bUseDX6Class)
        {
            /*
             * The number of texture stages was reduced to 1.
             * Use DX5 Class instead of DX6 class.
             */
            nvDX5TriangleSetup(pContext,dwPrimCount,pIndices,pVertices);
            return;
        }
    }
    nvSetDx6MultiTextureState(pContext);

    /*
     * calc appropriate render function
     */
    pContext->dwFunctionLookup = NV_FOG_MODE
                                      | ((pContext->dwRenderState[D3DRENDERSTATE_SHADEMODE] == D3DSHADE_FLAT) ? FLAG_FLAT : 0)
 #ifndef FLOAT_W
                                      | ((pContext->dwRenderState[D3DRENDERSTATE_ZENABLE] == D3DZB_USEW)      ? FLAG_FIXEDWBUF : 0)
 #endif
                                      | ((pContext->dwRenderState[D3DRENDERSTATE_SPECULARENABLE] == TRUE)     ? FLAG_SPECULAR : 0)
                                      | ((pDriverData->nvD3DPerfData.dwCPUFeatureSet & FS_MMX)    ? FLAG_MMX    : 0)
                                      | ((pDriverData->nvD3DPerfData.dwCPUFeatureSet & FS_KATMAI) ? FLAG_KATMAI : 0)
                                      | FLAG_DX6;

    if (pContext->pZetaBuffer)
    {
        CNvObject *pNvObj = pContext->pZetaBuffer->getWrapper();
        if (pNvObj->hasCTEnabled())
        {
            pContext->dwFunctionLookup |= pNvObj->isCTFront() ? FLAG_ZFP
                                                              : FLAG_ZBP;
        }
    }

    return;
}

/*
 * nvTriangleDispatch
 *
 * calls the triangle inner loop code
 *  - if it does not exist yet, build it
 */
void __stdcall nvTriangleDispatch
(
    NV_INNERLOOP_ARGLIST
)
{
    DWORD dwFLUI;
    DWORD dwOffset;
    DWORD dwTextureOrder;

    if (!dwCount) return; // do not waste time (or crash code that assume real work to be done)

    LPBYTE pVertices = (LPBYTE)(pContext->ppDX8Streams[0]->getAddress() + pContext->dp2.dwTotalOffset);
    LPWORD pIndices  = (LPWORD)(pContext->dp2.dwIndices);
    DWORD  dwStrides = nv4DP2OpToStride[pContext->dp2.dwDP2Prim];

    /*
    {
        extern BOOL nvPusherDisassemblerEnable;
        nvPusherDisassemblerEnable = 1;
    }
    */

    /*
     * get function pointer
     */
    dwFLUI = pContext->dwFunctionLookup;

    /*
     * if we have to call a fixed function, do it now
     *  current fixed functions are
     *      nvTriangleSetup
     *      nvAACapturePrimitive
     */
    if (dwFLUI & FLAG_DIRECT)
    {
        switch (dwFLUI)
        {
            case NV_AA_FUNCTION_INDEX:
            {
                return;
            }
            case NV_SETUP_FUNCTION_INDEX:
            {
                /*
                 * We need to call the correct state setup code based on what the application DX level is
                 * here if the hardware state has been changed, since a previous call to the hardware setup
                 * code has already potentially changed the DX5/DX6 object usage flag.
                 * DX6 applications must always start out calling the DX6 state setup code to insure the
                 * proper rendering state is used for hardware setup.
                 */
                if (pContext->bStateChange) {
                    ((pContext->dwDXAppVersion >= 0x0600) ? nvDX6TriangleSetup
                                                          : nvDX5TriangleSetup) (pContext,dwCount,pIndices,pVertices);
                }
                else {
                    (pContext->bUseDX6Class ? nvDX6TriangleSetup
                                            : nvDX5TriangleSetup) (pContext,dwCount,pIndices,pVertices);
                }

                /*
                 * reset dwFLUI
                 */
                dwFLUI = pContext->dwFunctionLookup;

                /*
                 * fall through so we can draw the primitives except for AA
                 */
                if (dwFLUI & FLAG_DIRECT) return;

                break;
            }
        }
    }

    /*
     * compute proper Lookup Index (LUI).
     *  nvTriangleSetup can only choose the major things like:
     *      dx5 or dx6
     *      fog
     *      (basic, mmx or katmai - determined at startup)
     *
     *  here we have to modify the pointer to deal with:
     *      indexed vs non-indexed
     *      fvf vs non fvf
     *      list,strip,fan
     */
    dwFLUI |= (pIndices ? FLAG_INDEXED : 0)
           |  ((dwStrides >> 24) << 2);
    if (dwFLUI & MASK_FOG)
    {
        dwFLUI |= (((pContext->surfaceViewport.dvWNear == 1.0f)
                 && (pContext->surfaceViewport.dvWFar  == 1.0f)) ? FLAG_ZFOG : 0);
    }

    /*
     * get function pointer
     */
    {
        PNVD3DMULTITEXTURESTATE pTriangleState = (PNVD3DMULTITEXTURESTATE)&pContext->mtsState;
        DWORD     dwIndex  = (pContext->pCurrentVShader->getFVF() + dwFLUI + (dwFLUI >> 12) + (dwFLUI >> 24)) & MASK_TABLE;
        CILHashEntry *pRec = (CILHashEntry*)dwDrawPrimitiveTable[dwIndex];

        dwTextureOrder = (pTriangleState->dwUVOffset[0] << 0)
                       | (pTriangleState->dwUVOffset[1] << 4);

#ifdef PRINT_NAME
        ILCDebug_printName (dwFLUI,fvfData.dwVertexType,dwTextureOrder);
#endif //PRINT_NAME

        /*
         * find proper loop
         */
        while (pRec)
        {
            if (pRec->match(dwFLUI, pContext->pCurrentVShader->getFVF(), dwTextureOrder))
            {
                dwOffset = pRec->getOffset();
                break;
            }
            pRec = pRec->getNext();
        }

        /*
         * not found?
         */
        if (!pRec)
        {
#ifdef PRINT_NAME_ON_BUILD
            ILCDebug_printName (dwFLUI,fvfData.dwVertexType,dwTextureOrder);
#endif //PRINT_NAME_ON_BUILD

            /*
             * build inner loop
             */
            dwOffset = ILCCompileCode(pContext, dwFLUI);
            pRec = new CILHashEntry (dwFLUI,
                                     pContext->pCurrentVShader->getFVF(),
                                     dwTextureOrder,
                                     dwOffset,
                                     (CILHashEntry*)dwDrawPrimitiveTable[dwIndex]);
            if (!pRec)
            {
                // out of memory
                DPF ("nvTriangleDispatch - out of memory");
                return;
            }
            dwDrawPrimitiveTable[dwIndex] = (DWORD)pRec;
        }
    }

    /*
     * set up inner loop variables
     */
    global.nvCachedPut       = getDC()->nvPusher.getPut();
    global.nvCachedThreshold = getDC()->nvPusher.getThreshold();
    global.dwPrimCount       = dwCount;
    global.pVertices         = (DWORD)pVertices;
    global.pIndices          = (DWORD)pIndices;
    global.fRWFar            = pContext->surfaceViewport.dvRWFar;
    global.dwSpecularCount   = 5;
    global.dwSpecularState   = 0;
    global.dwBlendRegister   = pContext->ctxInnerLoop.dwBlend;
#ifndef NOCULLING
    global.dwCullMask1 = (pContext->dwRenderState[D3DRENDERSTATE_CULLMODE] == D3DCULL_CCW)  ? 0x80000000 : 0x00000000;
    global.dwCullMask2 = (pContext->dwRenderState[D3DRENDERSTATE_CULLMODE] == D3DCULL_NONE) ? 0xffffffff : 0x80000000;
#endif

    if (dwFLUI & MASK_FOG)
    {
        global.pKNI->fFogTableDensity[0]     = pContext->fogData.fFogTableDensity;
        global.pKNI->fFogTableLinearScale[0] = pContext->fogData.fFogTableLinearScale;
        global.pKNI->fFogTableStart[0]       = pContext->fogData.fFogTableStart;
        global.pKNI->fFogTableEnd[0]         = pContext->fogData.fFogTableEnd;

        if (dwFLUI & FLAG_KATMAI)
        {
            #define PROP(x) x[1] = x[2] = x[3] = x[0]
            PROP(global.pKNI->fFogTableDensity);
            PROP(global.pKNI->fFogTableLinearScale);
            PROP(global.pKNI->fFogTableStart);
            PROP(global.pKNI->fFogTableEnd);
        }
    }

    if (pContext->pZetaBuffer)
    {
        CNvObject *pNvObj = pContext->pZetaBuffer->getWrapper();
        if (pNvObj->hasCTEnabled())
        {
            if ((pContext->dwRenderState[D3DRENDERSTATE_ZENABLE]      != D3DZB_TRUE)
             || (pContext->dwRenderState[D3DRENDERSTATE_ZWRITEENABLE] != TRUE))
            {
                pNvObj->tagCTZE();
            }
        }
        pNvObj->setCTDrawPrim();
    }

#ifdef PIX1
    {
        typedef struct
        {
            float x,y,z,rhw;
            DWORD c,s;
            float u0,v0,u1,v1;
        } SPECIAL;

        static SPECIAL vx0 = {0.0f,0.0f,0.5f,1.0f, 0x80ffffff,0x00000000, 0.0f,0.0f,0.0f,0.0f };
        static SPECIAL vx1 = {1.0f,0.0f,0.5f,1.0f, 0x80ffffff,0x00000000, 0.0f,0.0f,0.0f,0.0f };
        static SPECIAL vx2 = {0.0f,1.0f,0.5f,1.0f, 0x80ffffff,0x00000000, 0.0f,0.0f,0.0f,0.0f };

        DWORD dwCount = (dwFLUI & LUIFLAG_DX6) ? (sizeDx5TriangleTLVertex * 3)
                                               : (sizeDx6TriangleTLVertex * 3);
        DWORD dwSize  = (dwFLUI & LUIFLAG_DX6) ? 40 : 32;
        DWORD dwCode  = (dwFLUI & LUIFLAG_DX6) ? 0x0028e400 : 0x0020e400;
        DWORD dwBase  = (dwFLUI & LUIFLAG_DX6) ? 5 : 13;

        nvPushData (0, dwCode + dwBase * dwSize);
        for (i=0; i < dwSize>>2; i++)
            nvPushData (i+1, (DWORD *)(&vx0)[i]);
        nvPusherAdjust (dwSize>>2 + 1);

        dwBase ++;

        nvPushData (0, dwCode + dwBase * dwSize);
        for (i=0; i < dwSize>>2; i++)
            nvPushData (i+1, (DWORD *)(&vx1)[i]);
        nvPusherAdjust (dwSize>>2 + 1);

        dwBase ++;

        nvPushData (0, dwCode + dwBase * dwSize);
        for (i=0; i < dwSize>>2; i++)
            nvPushData (i+1, (DWORD *)(&vx2)[i]);
        nvPusherAdjust (dwSize>>2 + 1);

    }
#endif //PIX1

    //
    // setup mush instrumentation
    //
    if (pDriverData->nvD3DPerfData.dwPerformanceStrategy & PS_MUSH)
    {
        global.fTotalPixelsTimes2 = 0.0f;
    }

    /*
     * draw it
     */
#ifdef INSTRUMENTED
    {
#ifdef PLOT
        static BOOL first = 1;
#endif
        __int64 time1;
        __int64 time2;
        DWORD   put1 = (DWORD)global.nvCachedPut;
        DWORD   put2;

#ifdef PLOT
        if (first)
        {
            ILCDebug_cls();
            first = 0;
        }
#endif

        __asm
        {
            pushad
            rdtsc
            mov [dword ptr time1 + 0],eax
            mov [dword ptr time1 + 4],edx
            popad
        }
#endif //INSTRUMENTED

    //
    // call inner loop
    //
    ((PFNINNERLOOP)(ilcData + dwOffset))();
#ifndef NV_NULL_HW_DRIVER
    getDC()->nvPusher.setPut (global.nvCachedPut);
#endif

#ifdef INSTRUMENTED
        __asm
        {
            pushad
            rdtsc
            mov [dword ptr time2 + 0],eax
            mov [dword ptr time2 + 4],edx
            popad
        }

        put2 = (DWORD)global.nvCachedPut;

        if (put2 > put1)
        {
            static double total = 0.0f;
            static double count = 0.0f;
            static double min   = 1e20f;
            static double max   = 0.0f;
#ifndef WINNT   // WINNT timer routines need a larger datatype
            static DWORD  last  = 0;
#else   // WINNT
            LONGLONG last = 0;
#endif  // WINNT

            double bps = (double)(put2 - put1) * (double)(500.0) / (double)(time2 - time1);
#ifndef WINNT   // WINNT timer routines need a larger datatype
            DWORD now;
#else   // WINNT
            LONGLONG now;
#endif  // WINNT

            //global.nvCachedPut = put1;

            if (bps < min)
            {
                min = bps;
                if (0)//(min < 5.0f)
                {
                    DWORD cycles = (DWORD)(time2 - time1);
                    DWORD bytes  = (DWORD)(put2  - put1);
                    DPF ("*** low: %d bytes, %d cycles, %d cycles / byte", bytes,cycles,cycles/bytes);
                }
            }
            if (bps > max)
            {
                max = bps;
                if (0)//(max > 250.0f)
                {
                    DWORD cycles = (DWORD)(time2 - time1);
                    DWORD bytes  = (DWORD)(put2  - put1);
                    DPF ("*** high: %d bytes, %d cycles, %d cycles / byte", bytes,cycles,cycles/bytes);
                }
            }

            total += bps;
            count += 1.0f;

#ifdef PLOT
            {
                double x = 720.0 / (64.0 * 1024.0) * (double)(put2 - put1);
                double y = 347.0 - (348.0 / 800.0 * bps);
                int    c = 0;
                if (dwFLUI & MASK_FOG)  c = 1;
                if (dwFLUI & FLAG_FLAT) c = 2;

                if (y < 0.0)   y = 0.0;
                if (x > 719.0) x = 719.0;

                ILCDebug_plot ((int)x,(int)y,c);
                DPF ("%d %d", (int)x,(int)y);
            }
#else
#ifndef WINNT   // Can't macroize this one
            now = GetTickCount();
#else   // WINNT
            EngQueryPerformanceCounter(&now);
#endif  // WINNT

            if ((now - last) > 1500)
            {
                DPF ("%08x: cnt:%d bps:%d max:%d min:%d\n",
                     ((DWORD)global.nvCachedPut - (DWORD)getDC()->nvPusher.getBase()),
                     (DWORD)count,
                     (DWORD)(total / count),
                     (DWORD)max,
                     (DWORD)min);
                last  = now;
                total = 0.0f;
                count = 0.0f;
                max   = 0.0f;
                min   = 1e20f;
            }
#endif
        }
    }
#endif //INSTRUMENTED

    nvPusherAdjust (0);

    //
    // wrap up mush instrumentation
    //
    if (pDriverData->nvD3DPerfData.dwPerformanceStrategy & PS_MUSH)
    {
        //
        // program delay
        //
        double fdScale  = (GET_MODE_BPP() > 16) ? 2 : 1;
        double fdPixels = fabs(global.fTotalPixelsTimes2) * 0.5;
        double fdMin1   = dwCount * 16.0;
        double fdMin2   = fdPixels * ((dwFLUI & FLAG_DX6) ? 1.0 : 0.5);
        double fdWait;
        if (fdMin1 > fdMin2)
        {
            // setup bound
            fdWait = fdScale * global.fdClockPerTri * dwCount - fdMin1;
        }
        else
        {
            // fill bound
            fdWait = fdScale * global.fdClockPerPixel * fdPixels - fdMin2;
        }
        // send to HW (assume dx5/dx6 triangle selected)
        if (fdWait >= 25.0)
        {
            // send wait
            DWORD dwWait = DWORD(fdWait);
            nvPushData (0,(1 << 18) | (NV_DD_SPARE << 13) | NV054_STALL_PIPE);
            nvPushData (1,dwWait);
            nvPusherAdjust (2);
            // make HW look busy
            /*
            for (int i = 0; i < 25; i++)
            {
                nvPushData (0,(1 << 18) | (NV_DD_SPARE << 13) | NV054_NO_OPERATION);
                nvPushData (1,dwWait);
                nvPusherAdjust (2);
            }
            */
        }
    }

    /*
    if (dwFLUI & FLAG_DX6)
    {
        DPF ("6");
    }
    else
    {

        DWORD drfValueFilter;
        DWORD drfValueSpec;
        DWORD drfValueAlpha;
        DWORD drfValueFog;
        drfValueFilter = DRF_VAL(054, _FILTER, _TEXTUREMIN, pContext->ctxInnerLoop.dwFilter);
        drfValueSpec = DRF_VAL(054, _BLEND, _SPECULARENABLE, pContext->ctxInnerLoop.dwBlend);
        drfValueAlpha = DRF_VAL(054, _BLEND, _ALPHABLENDENABLE, pContext->ctxInnerLoop.dwBlend);
        drfValueFog = DRF_VAL(054, _BLEND, _FOGENABLE, pContext->ctxInnerLoop.dwBlend);

        if ((drfValueFilter == NV054_FILTER_TEXTUREMIN_LINEARMIPNEAREST) ||
            (drfValueFilter == NV054_FILTER_TEXTUREMIN_LINEARMIPLINEAR)) {
            DPF ("T");

        } else if (drfValueSpec == NV054_BLEND_SPECULARENABLE_TRUE) {
            DPF ("s");
        } else if (drfValueAlpha == NV054_BLEND_ALPHABLENDENABLE_TRUE &&
                   drfValueFog == NV054_BLEND_FOGENABLE_TRUE) {
            DPF ("a");
        } else {
            DPF ("_");
        }
    }
//*/

}


/*****************************************************************************

 *****************************************************************************

 *****************************************************************************

 *****************************************************************************

 *****************************************************************************

 *****************************************************************************

 *****************************************************************************

 *****************************************************************************

 *****************************************************************************

 *****************************************************************************

 *****************************************************************************

 *****************************************************************************

 *****************************************************************************

 *****************************************************************************

 *****************************************************************************

 *****************************************************************************

 *****************************************************************************/

DWORD nvBuildCustomCopyRoutine
(
    DWORD dwCount
)
{
    // cannot return zero offset (special meaning)
    if (!ilcCount) { xINT3 };

    // align to 32 byte boundary
    while (ilcCount & 31) { xINT3 }

    DWORD label_entry;
    xLABEL (label_entry)

    // get src & dest
    xPUSH_r     (rEDI)
    xPUSH_r     (rESI)
    xMOV_r_rm   (rEDI,rmSIB8) xSIB(rESP,rESP,x1) xOFS8 (12)
    xMOV_r_rm   (rESI,rmSIB8) xSIB(rESP,rESP,x1) xOFS8 (16)

    // do copy
    if (pDriverData->nvD3DPerfData.dwCPUFeatureSet & FS_WILLAMETTE)
    {
        //
        // WNI copy
        //
        DWORD i = 0;
        while (dwCount >= 64)
        {
            xMOVSD_r_rm     (rXMM0,rmIND8(rESI)) xOFS8(i +  0)
            xMOVSD_rm_r     (rmIND8(rEDI),rXMM0) xOFS8(i +  0)
            xMOVSD_r_rm     (rXMM1,rmIND8(rESI)) xOFS8(i +  8)
            xMOVSD_rm_r     (rmIND8(rEDI),rXMM1) xOFS8(i +  8)

            xMOVSD_r_rm     (rXMM2,rmIND8(rESI)) xOFS8(i + 16)
            xMOVSD_rm_r     (rmIND8(rEDI),rXMM2) xOFS8(i + 16)
            xMOVSD_r_rm     (rXMM3,rmIND8(rESI)) xOFS8(i + 24)
            xMOVSD_rm_r     (rmIND8(rEDI),rXMM3) xOFS8(i + 24)

            xMOVSD_r_rm     (rXMM4,rmIND8(rESI)) xOFS8(i + 32)
            xMOVSD_rm_r     (rmIND8(rEDI),rXMM4) xOFS8(i + 32)
            xMOVSD_r_rm     (rXMM5,rmIND8(rESI)) xOFS8(i + 40)
            xMOVSD_rm_r     (rmIND8(rEDI),rXMM5) xOFS8(i + 40)

            xMOVSD_r_rm     (rXMM6,rmIND8(rESI)) xOFS8(i + 48)
            xMOVSD_rm_r     (rmIND8(rEDI),rXMM6) xOFS8(i + 48)
            xMOVSD_r_rm     (rXMM7,rmIND8(rESI)) xOFS8(i + 56)
            xMOVSD_rm_r     (rmIND8(rEDI),rXMM7) xOFS8(i + 56)
            dwCount -= 64;
            i       += 64;
            if (i >= 128)
            {
                xADD_rm_imm (rmREG(rESI),i)
                xADD_rm_imm (rmREG(rEDI),i)
                i = 0;
            }
        }

        while (dwCount >= 16)
        {
            xMOVSD_r_rm     (rXMM0,rmIND8(rESI)) xOFS8(i)
            xMOVSD_rm_r     (rmIND8(rEDI),rXMM0) xOFS8(i)
            xMOVSD_r_rm     (rXMM0,rmIND8(rESI)) xOFS8(i+8)
            xMOVSD_rm_r     (rmIND8(rEDI),rXMM0) xOFS8(i+8)
            dwCount -= 16;
            i       += 16;
            if (i >= 128)
            {
                xADD_rm_imm (rmREG(rESI),i)
                xADD_rm_imm (rmREG(rEDI),i)
                i = 0;
            }
        }

        while (dwCount >= 8)
        {
            xMOV_r_rm    (rEAX,rmIND8(rESI)) xOFS8(i)
            xMOV_rm_r    (rmIND8(rEDI),rEAX) xOFS8(i)
            xMOV_r_rm    (rECX,rmIND8(rESI)) xOFS8(i + 4)
            xMOV_rm_r    (rmIND8(rEDI),rECX) xOFS8(i + 4)
            dwCount -= 8;
            i       += 8;
            if (i >= 128)
            {
                xADD_rm_imm (rmREG(rESI),i)
                xADD_rm_imm (rmREG(rEDI),i)
                i = 0;
            }
        }

        while (dwCount >= 4)
        {
            xMOV_r_rm    (rEAX,rmIND8(rESI)) xOFS8(i)
            xMOV_rm_r    (rmIND8(rEDI),rEAX) xOFS8(i)
            dwCount -= 4;
            i       += 4;
            if (i >= 128)
            {
                xADD_rm_imm (rmREG(rESI),i)
                xADD_rm_imm (rmREG(rEDI),i)
                i = 0;
            }
        }

        if (dwCount)
        {
            xADD_rm_imm (rmREG(rESI),i)
            xADD_rm_imm (rmREG(rEDI),i)
            xMOV_rm_imm (rmREG(rECX),dwCount)
            xREP xMOVSB
        }
    }
    else if (pDriverData->nvD3DPerfData.dwCPUFeatureSet & FS_KATMAI)
    {
        //
        // KNI copy
        //
        DWORD i = 0;
        while (dwCount >= 64)
        {
            xMOVLPS_r_rm    (rXMM0,rmIND8(rESI)) xOFS8(i +  0)
            xMOVHPS_r_rm    (rXMM0,rmIND8(rESI)) xOFS8(i +  8)
            xMOVLPS_r_rm    (rXMM1,rmIND8(rESI)) xOFS8(i + 16)
            xMOVHPS_r_rm    (rXMM1,rmIND8(rESI)) xOFS8(i + 24)
            xMOVLPS_r_rm    (rXMM2,rmIND8(rESI)) xOFS8(i + 32)
            xMOVHPS_r_rm    (rXMM2,rmIND8(rESI)) xOFS8(i + 40)
            xMOVLPS_r_rm    (rXMM3,rmIND8(rESI)) xOFS8(i + 48)
            xMOVHPS_r_rm    (rXMM3,rmIND8(rESI)) xOFS8(i + 56)
            xMOVLPS_rm_r    (rmIND8(rEDI),rXMM0) xOFS8(i +  0)
            xMOVHPS_rm_r    (rmIND8(rEDI),rXMM0) xOFS8(i +  8)
            xMOVLPS_rm_r    (rmIND8(rEDI),rXMM1) xOFS8(i + 16)
            xMOVHPS_rm_r    (rmIND8(rEDI),rXMM1) xOFS8(i + 24)
            xMOVLPS_rm_r    (rmIND8(rEDI),rXMM2) xOFS8(i + 32)
            xMOVHPS_rm_r    (rmIND8(rEDI),rXMM2) xOFS8(i + 40)
            xMOVLPS_rm_r    (rmIND8(rEDI),rXMM3) xOFS8(i + 48)
            xMOVHPS_rm_r    (rmIND8(rEDI),rXMM3) xOFS8(i + 56)
            dwCount -= 64;
            i       += 64;
            if (i >= 128)
            {
                xADD_rm_imm (rmREG(rESI),i)
                xADD_rm_imm (rmREG(rEDI),i)
                i = 0;
            }
        }

        while (dwCount >= 16)
        {
            xMOVLPS_r_rm    (rXMM0,rmIND8(rESI)) xOFS8(i)
            xMOVHPS_r_rm    (rXMM0,rmIND8(rESI)) xOFS8(i+8)
            xMOVLPS_rm_r    (rmIND8(rEDI),rXMM0) xOFS8(i)
            xMOVHPS_rm_r    (rmIND8(rEDI),rXMM0) xOFS8(i+8)
            dwCount -= 16;
            i       += 16;
            if (i >= 128)
            {
                xADD_rm_imm (rmREG(rESI),i)
                xADD_rm_imm (rmREG(rEDI),i)
                i = 0;
            }
        }

        while (dwCount >= 8)
        {
            xMOV_r_rm    (rEAX,rmIND8(rESI)) xOFS8(i)
            xMOV_r_rm    (rECX,rmIND8(rESI)) xOFS8(i + 4)
            xMOV_rm_r    (rmIND8(rEDI),rEAX) xOFS8(i)
            xMOV_rm_r    (rmIND8(rEDI),rECX) xOFS8(i + 4)
            dwCount -= 8;
            i       += 8;
            if (i >= 128)
            {
                xADD_rm_imm (rmREG(rESI),i)
                xADD_rm_imm (rmREG(rEDI),i)
                i = 0;
            }
        }

        while (dwCount >= 4)
        {
            xMOV_r_rm    (rEAX,rmIND8(rESI)) xOFS8(i)
            xMOV_rm_r    (rmIND8(rEDI),rEAX) xOFS8(i)
            dwCount -= 4;
            i       += 4;
            if (i >= 128)
            {
                xADD_rm_imm (rmREG(rESI),i)
                xADD_rm_imm (rmREG(rEDI),i)
                i = 0;
            }
        }

        if (dwCount)
        {
            xADD_rm_imm (rmREG(rESI),i)
            xADD_rm_imm (rmREG(rEDI),i)
            xMOV_rm_imm (rmREG(rECX),dwCount)
            xREP xMOVSB
        }
    }
    else if (pDriverData->nvD3DPerfData.dwCPUFeatureSet & FS_ATHLON)
    {
        //
        // Athlon copy
        //
        bool needFemms = false;
        if (dwCount >= 16)
        {
            xFEMMS
            needFemms = true;
        }
        DWORD i = 0;
        while (dwCount >= 64)
        {
            xMOVQ_r_rm  (rMM0, rmIND8(rESI)) xOFS8(i + 0)
            xMOVQ_r_rm  (rMM1, rmIND8(rESI)) xOFS8(i + 8)
            xMOVQ_r_rm  (rMM2, rmIND8(rESI)) xOFS8(i + 16)
            xMOVQ_r_rm  (rMM3, rmIND8(rESI)) xOFS8(i + 24)
            xMOVQ_r_rm  (rMM4, rmIND8(rESI)) xOFS8(i + 32)
            xMOVQ_r_rm  (rMM5, rmIND8(rESI)) xOFS8(i + 40)
            xMOVQ_r_rm  (rMM6, rmIND8(rESI)) xOFS8(i + 48)
            xMOVQ_r_rm  (rMM7, rmIND8(rESI)) xOFS8(i + 56)
            xMOVNTQ_rm_r (rmIND8(rEDI), rMM0) xOFS8(i + 0)
            xMOVNTQ_rm_r (rmIND8(rEDI), rMM1) xOFS8(i + 8)
            xMOVNTQ_rm_r (rmIND8(rEDI), rMM2) xOFS8(i + 16)
            xMOVNTQ_rm_r (rmIND8(rEDI), rMM3) xOFS8(i + 24)
            xMOVNTQ_rm_r (rmIND8(rEDI), rMM4) xOFS8(i + 32)
            xMOVNTQ_rm_r (rmIND8(rEDI), rMM5) xOFS8(i + 40)
            xMOVNTQ_rm_r (rmIND8(rEDI), rMM6) xOFS8(i + 48)
            xMOVNTQ_rm_r (rmIND8(rEDI), rMM7) xOFS8(i + 56)
            dwCount -= 64;
            i       += 64;
            if (i >= 128)
            {
                xADD_rm_imm (rmREG(rESI),i)
                xADD_rm_imm (rmREG(rEDI),i)
                i = 0;
            }
        }

        while (dwCount >= 16)
        {
            xMOVQ_r_rm  (rMM0, rmIND8(rESI)) xOFS8(i + 0)
            xMOVQ_r_rm  (rMM1, rmIND8(rESI)) xOFS8(i + 8)
            xMOVNTQ_rm_r (rmIND8(rEDI), rMM0) xOFS8(i + 0)
            xMOVNTQ_rm_r (rmIND8(rEDI), rMM1) xOFS8(i + 8)
            dwCount -= 16;
            i       += 16;
            if (i >= 128)
            {
                xADD_rm_imm (rmREG(rESI),i)
                xADD_rm_imm (rmREG(rEDI),i)
                i = 0;
            }
        }

        while (dwCount >= 8)
        {
            xMOV_r_rm    (rEAX,rmIND8(rESI)) xOFS8(i)
            xMOV_rm_r    (rmIND8(rEDI),rEAX) xOFS8(i)
            xMOV_r_rm    (rECX,rmIND8(rESI)) xOFS8(i + 4)
            xMOV_rm_r    (rmIND8(rEDI),rECX) xOFS8(i + 4)
            dwCount -= 8;
            i       += 8;
            if (i >= 128)
            {
                xADD_rm_imm (rmREG(rESI),i)
                xADD_rm_imm (rmREG(rEDI),i)
                i = 0;
            }
        }

        while (dwCount >= 4)
        {
            xMOV_r_rm    (rEAX,rmIND8(rESI)) xOFS8(i)
            xMOV_rm_r    (rmIND8(rEDI),rEAX) xOFS8(i)
            dwCount -= 4;
            i       += 4;
            if (i >= 128)
            {
                xADD_rm_imm (rmREG(rESI),i)
                xADD_rm_imm (rmREG(rEDI),i)
                i = 0;
            }
        }

        if (dwCount)
        {
            xADD_rm_imm (rmREG(rESI),i)
            xADD_rm_imm (rmREG(rEDI),i)
            xMOV_rm_imm (rmREG(rECX),dwCount)
            xREP xMOVSB
        }

        if (needFemms)
        {
            xFEMMS
        }
    }
    else
    {
        //
        // normal 8086 copy
        //
        DWORD dwWordCount = dwCount / 4;
        if (dwWordCount)
        {
            xMOV_rm_imm (rmREG(rECX),dwWordCount)
            xREP xMOVSD
        }
        dwCount &= 3;
        if (dwCount)
        {
            xMOV_rm_imm (rmREG(rECX),dwCount)
            xREP xMOVSB
        }
    }

    // done
    xPOP_r      (rESI)
    xPOP_r      (rEDI)
    xRET

    return label_entry;
}


DWORD nvBuildCustomCopyRoutineMisAligned
(
)
{
    // cannot return zero offset (special meaning)
    if (!ilcCount) { xINT3 };

    // align to 32 byte boundary
    while (ilcCount & 31) { xINT3 }

    DWORD label_entry;
    xLABEL (label_entry)

    // get src & dest
    xPUSH_r     (rEDI)
    xPUSH_r     (rESI)
    xPUSH_r     (rEBX)
    xMOV_r_rm   (rEDI,rmSIB8) xSIB(rESP,rESP,x1) xOFS8 (16)
    xMOV_r_rm   (rESI,rmSIB8) xSIB(rESP,rESP,x1) xOFS8 (20)
    xMOV_r_rm   (rECX,rmSIB8) xSIB(rESP,rESP,x1) xOFS8 (24)

    if (pDriverData->nvD3DPerfData.dwCPUFeatureSet & FS_ATHLON)
    {
        xFEMMS
    }
    else
    {
        xEMMS
    }

    DWORD line_size = 64;       //todo: base this on the cpu type
    DWORD distance =  2*64;      //todo: base this on the cpue type
    if (pDriverData->nvD3DPerfData.dwCPUFeatureSet & FS_WILLAMETTE)
        distance = 5 * 64;

    xMOV_r_rm   (rEDX, rmREG(rESI))
    xAND_rm_imm (rmREG(rEDX), ~(line_size-1))
    xPREFETCH_rm8   (1, rmIND8(rEDX), 0)
    xPREFETCH_rm8   (1, rmIND8(rEDX), 64)
    xADD_rm_imm (rmREG(rEDX), distance)
    xMOV_r_rm   (rEBX, rmREG(rEDI))
    xMOV_rm_imm (rmREG(rEAX), line_size)
    xAND_rm_imm (rmREG(rEBX), line_size-1)
    xSUB_r_rm   (rEAX, rmREG(rEBX))
    xAND_rm_imm (rmREG(rEAX), line_size-1)
    xTEST_r_rm  (rEAX, rmREG(rEAX))


    DWORD copyloop;
    DWORD aligned;
    xLABEL  (aligned)
    xJZ32   (0)

    xMOV_r_rm   (rECX, rmREG(rEAX))
    xREP    xMOVSB
    xMOV_r_rm   (rECX,rmSIB8) xSIB(rESP,rESP,x1) xOFS8 (24)
    xSUB_r_rm   (rECX, rmREG(rEAX))
    DWORD finishup;
    xCMP_rm_imm (rmREG(rECX), 64)
    xLABEL  (finishup)
    xJL32   (0)

    xTARGET_b32 (aligned)
    xLABEL (copyloop)
    xPREFETCH_rm8   (1, rmIND8(rEDX), 0)

    xMOVQ_r_rm  (rMM0, rmIND(rESI))
    xMOVQ_r_rm  (rMM1, rmIND8(rESI)) xOFS8(8)
    xMOVQ_r_rm  (rMM2, rmIND8(rESI)) xOFS8(16)
    xMOVQ_r_rm  (rMM3, rmIND8(rESI)) xOFS8(24)
    xMOVQ_r_rm  (rMM4, rmIND8(rESI)) xOFS8(32)
    xMOVQ_r_rm  (rMM5, rmIND8(rESI)) xOFS8(40)
    xMOVQ_r_rm  (rMM6, rmIND8(rESI)) xOFS8(48)
    xMOVQ_r_rm  (rMM7, rmIND8(rESI)) xOFS8(56)
    xADD_rm_imm (rmREG(rESI), 64)
    xMOVNTQ_rm_r (rmIND(rEDI), rMM0)
    xMOVNTQ_rm_r (rmIND8(rEDI), rMM1) xOFS8(8)
    xMOVNTQ_rm_r (rmIND8(rEDI), rMM2) xOFS8(16)
    xMOVNTQ_rm_r (rmIND8(rEDI), rMM3) xOFS8(24)
    xMOVNTQ_rm_r (rmIND8(rEDI), rMM4) xOFS8(32)
    xMOVNTQ_rm_r (rmIND8(rEDI), rMM5) xOFS8(40)
    xMOVNTQ_rm_r (rmIND8(rEDI), rMM6) xOFS8(48)
    xMOVNTQ_rm_r (rmIND8(rEDI), rMM7) xOFS8(56)
    xADD_rm_imm (rmREG(rEDI), 64)
    xADD_rm_imm (rmREG(rEDX), 64)
    xSUB_rm_imm (rmREG(rECX), 64)
    xCMP_rm_imm (rmREG(rECX), 64)
    xJG32   (copyloop)

    xTARGET_b32(finishup)
    xTEST_r_rm  (rECX, rmREG(rECX))
    DWORD done;
    xLABEL  (done)
    xJLE    (0)

    xREP    xMOVSB

    xTARGET_b8(done)

    if (pDriverData->nvD3DPerfData.dwCPUFeatureSet & FS_ATHLON)
    {
        xFEMMS
    }
    else
    {
        xEMMS
    }
    xPOP_r      (rEBX)
    xPOP_r      (rESI)
    xPOP_r      (rEDI)
    xRET

    return label_entry;
}

#endif  // NVARCH >= 0x04

