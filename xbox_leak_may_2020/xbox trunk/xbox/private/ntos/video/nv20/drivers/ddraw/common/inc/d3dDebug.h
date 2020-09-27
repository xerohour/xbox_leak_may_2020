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
//  Module: d3dDebug.h
//      D3D-specific debugging routines
//
// **************************************************************************
//
//  History:
//      Chas Inman              18Nov97         original version
//      Craig Duttweiler        21Nov00         major rehash
//
// **************************************************************************

#ifndef _NV3DDBG_H_
#define _NV3DDBG_H_

#ifdef __cplusplus
extern "C" {
#endif

//-------------------------------------------------------------------------
// SHARED DEBUG CODE CONFIGURATION
//-------------------------------------------------------------------------

// override the defaults
#undef  DPF_START_STR
#define DPF_START_STR  "NVDD32: "

#undef  DPF_END_STR
#define DPF_END_STR    "\r\n"

#undef  DPF_LOG_FILE
#define DPF_LOG_FILE   "\\nvdd32.log"

// definitions for dbgLevel debug output level flags
#define NVDBG_LEVEL_ERROR               0x80000000
#define NVDBG_LEVEL_INFO                0x40000000
#define NVDBG_LEVEL_VERTEX_REUSE        0x20000000
#define NVDBG_LEVEL_COLORKEY_INFO       0x10000000
#define NVDBG_LEVEL_AA_INFO             0x08000000
#define NVDBG_LEVEL_FLIP                0x04000000
#define NVDBG_LEVEL_SURFACE             0x02000000  // shows info for CreateSurface, CreateSurfaceEx and DestroySurface
#define NVDBG_LEVEL_SURFACE_ACTIVITY    0x01000000  // shows surface activity
#define NVDBG_LEVEL_SURFACEALLOC        0x00800000  // print info about surface allocations
#define NVDBG_LEVEL_NVOBJ_INFO          0x00400000  // print reference/release access on CNVObjects
#define NVDBG_LEVEL_TEXMAN              0x00200000  // print info about texture fetches and evictions
#define NVDBG_LEVEL_HEAP_ACTIVITY       0x00100000  // print info about IPM heap activity
#define NVDBG_LEVEL_HEAP_USAGE          0x00080000  // print info about heap usage
#define NVDBG_LEVEL_PALETTE             0x00040000  // print info about palette usage
#define NVDBG_LEVEL_PERFORMANCE         0x00020000  // print performance issues
#define NVDBG_LEVEL_VSHADER_INFO        0x00010000  // print inner loop info about vertex shaders
#define NVDBG_LEVEL_VSHADER_IO          0x00008000  // print inner loop info about vertex shader input/output data
#define NVDBG_LEVEL_VSHADER_INS         0x00004000  // print vertex shader instructions
#define NVDBG_LEVEL_HOS_INFO            0x00002000  // print info on HOS processing
#define NVDBG_LEVEL_HOS_VERTEX_INFO     0x00001000  // print info on HOS vertex data
#define NVDBG_LEVEL_PIXEL_SHADER        0x00000800  // print pixel shader instructions (create info)
#define NVDBG_LEVEL_NV_AGP              0x00000400  // print info about nvidia managed AGP activity
#ifdef KPFS
#define NVDBG_LEVEL_VSHADER_MAN         0x00000200  // print info about the vertex shader manager
#endif

//-------------------------------------------------------------------------
// D3D DEBUG BITS
//-------------------------------------------------------------------------

// Bit definitions for the dbgShowState debug flags
#define NVDBG_SHOW_RENDER_STATE         0x80000000
#define NVDBG_SHOW_DX5_CLASS_STATE      0x40000000
#define NVDBG_SHOW_SURFACE_STATE        0x20000000
#define NVDBG_SHOW_VERTICES             0x10000000
#define NVDBG_SHOW_MTS_STATE            0x0F000000  // Selects number of multi-texture stage state values to display.
#define NVDBG_SHOW_MTS_STATE_MAX        8
#define NVDBG_SHOW_MTS_STATE_SHIFT      24
#define NVDBG_SHOW_DX6_CLASS_STATE      0x00800000
#define NVDBG_SHOW_DRAWPRIM2_INFO       0x00400000
#define NVDBG_SHOW_VERTEX_MAX_MIN       0x00200000
#define NVDBG_SHOW_LIGHTS               0x00100000
#define NVDBG_SHOW_MANUAL_OVERRIDE      0x00000002  // force show/dump regardless of dbgShowState/dbgDumpState values
#define NVDBG_SHOW_TEXTURE_RENAME       0x00080000  // prints information on texture renaming

// Bit definitions for the dbgFlush debug flags
#define NVDBG_FLUSH_PRIMITIVE           0x80000000  // flush triangles, lines, points, etc.
#define NVDBG_FLUSH_SETUP               0x40000000  // flush commands that set up / tear down HW object state
#define NVDBG_FLUSH_STATE               0x20000000  // flush DP2 commands that set render state, etc
#define NVDBG_FLUSH_2D                  0x10000000  // flush 2d operations: blits, clears, flips, etc.

// Bit definitions for the texture preview debug flags
#define NVDBG_TEXTURE_PREVIEW_BASE      0x00000001
#define NVDBG_TEXTURE_PREVIEW_MIP       0x00000002
#define NVDBG_TEXTURE_PREVIEW_CUBE      0x00000004
#define NVDBG_TEXTURE_PREVIEW_LINEAR    0x80000000  // preview linear rather than swizzled version
#define NVDBG_TEXTURE_PREVIEW_ALPHA     0x08000000  // preview the alpha values in grayscale

// Filenames for dumped data
#if 0
// multiple small logs
#define NVDBG_DUMP_RENDER_STATE_FILENAME     "\\render_state.txt"
#define NVDBG_DUMP_DX5_CLASS_STATE_FILENAME  "\\dx5_state.txt"
#define NVDBG_DUMP_SURFACE_STATE_FILENAME    "\\surface_state.txt"
#define NVDBG_DUMP_VERTICES_FILENAME         "\\vertices.txt"
#define NVDBG_DUMP_MTS_STATE_FILENAME        "\\mts_state.txt"
#define NVDBG_DUMP_DX6_CLASS_STATE_FILENAME  "\\dx6_state.txt"
#define NVDBG_DUMP_DRAWPRIM2_INFO_FILENAME   "\\drawprim2_info.txt"
#define NVDBG_DUMP_MANUAL_OVERRIDE_FILENAME  "\\manual_dump.txt"
#else
// one big log
#define NVDBG_DUMP_RENDER_STATE_FILENAME     "\\d3ddump.txt"
#define NVDBG_DUMP_DX5_CLASS_STATE_FILENAME  "\\d3ddump.txt"
#define NVDBG_DUMP_SURFACE_STATE_FILENAME    "\\d3ddump.txt"
#define NVDBG_DUMP_VERTICES_FILENAME         "\\d3ddump.txt"
#define NVDBG_DUMP_MTS_STATE_FILENAME        "\\d3ddump.txt"
#define NVDBG_DUMP_DX6_CLASS_STATE_FILENAME  "\\d3ddump.txt"
#define NVDBG_DUMP_DRAWPRIM2_INFO_FILENAME   "\\d3ddump.txt"
#define NVDBG_DUMP_MANUAL_OVERRIDE_FILENAME  "\\d3ddump.txt"
#endif

//-------------------------------------------------------------------------
// GLOBAL VARIABLES
//-------------------------------------------------------------------------

#ifdef DEBUG

    // general debugging
    extern  DWORD                   dbgShowState;
    extern  DWORD                   dbgDumpState;
    extern  DWORD                   dbgFlush;
    extern  DWORD                   dbgForceHWRefresh;

    // primary info (shared by front buffer rendering and texture previewing)
    extern  DWORD                   dbgPrimaryPitch;
    extern  DWORD                   dbgPrimaryHeight;
    extern  DWORD                   dbgPrimaryBPP;

    // front buffer rendering
    extern  BOOL                    dbgFrontRender;
    extern  CSimpleSurface          dbgFrontRenderSurface;

    // texture previewing
    extern  DWORD                   dbgTexturePreview;

#else

    #define dbgShowState            0
    #define dbgDumpState            0
    #define dbgFlush                0
    #define dbgForceHWRefresh       0

    // primary info (shared by front buffer rendering and texture previewing)
    #define dbgPrimaryPitch         0
    #define dbgPrimaryHeight        0
    #define dbgPrimaryBPP           0

    #define dbgFrontRender          FALSE
    #define dbgFrontRenderSurface   NULL

    #define dbgTexturePreview       FALSE

#endif

//-------------------------------------------------------------------------
// PROTOTYPES
//-------------------------------------------------------------------------

#ifdef DEBUG

    // printing
    void __cdecl dbgDPFAndDump                  (DWORD flag, LPSTR szFormat, ...);

    // state dumping
    void         dbgDisplayContextState         (DWORD, PNVD3DCONTEXT);
    void         dbgShowRenderStateChange       (BOOL bChanged, DWORD dwStateType, DWORD dwStateValue);
    void         dbgDisplayDrawPrimitives2Info  (BYTE,WORD);
    void         dbgDisplayVertexData           (BOOL, DWORD *);
    void         dbgDisplayLight                (LPD3DLIGHT7 pLight7);

    // state validation
    BOOL         dbgValidateControlRegisters    (PNVD3DCONTEXT);

    // texture previewing
    void         dbgTextureDisplay              (CTexture *pTexture);

    // front rendering
    void         dbgSetFrontRenderTarget        (PNVD3DCONTEXT pContext, CSimpleSurface *pSurfaceToMimic);

#else  // !DEBUG

    // printing
    #define      dbgDPFAndDump                  1 ? (void)0 : (void)

    // state dumping
    #define      dbgDisplayContextState(f,x)
    #define      dbgShowRenderStateChange(a,b,c)
    #define      dbgDisplayDrawPrimitives2Info(x,y)
    #define      dbgDisplayVertexData(a,b)
    #define      dbgDisplayLight(pLight7)

    // state validation
    #define      dbgValidateControlRegisters(noop) TRUE

    // texture previewing
    #define      dbgTextureDisplay(a)

    // front rendering
    #define      dbgSetFrontRenderTarget(a,b)

#endif

#ifdef NVSTATDRIVER

#define DX7PROPER 1

// prototypes
void __cdecl nvStatLogError(DWORD dwLevel, LPSTR szFormat, ...);

// override some stuff
#undef DPF_LEVEL
#define DPF_LEVEL nvStatLogError
#undef  dbgDisplayVertexData
#define dbgDisplayVertexData nvStatLogVertexData
#endif  // NVSTATDRIVER


//-------------------------------------------------------------------------
// MISCELLANEOUS
//-------------------------------------------------------------------------

#ifdef DEBUG

    #define dbgD3DError()  dbgError("NV D3D ERROR!");
    #define dbgFlushType(type)                                                  \
    {                                                                           \
        if (dbgFlush & type) {                                                  \
            getDC()->nvPusher.flush (TRUE, CPushBuffer::FLUSH_WITH_DELAY);      \
        }                                                                       \
    }

#elif defined(NVSTATDRIVER)

    #define dbgD3DError()  DPF("NV D3D ERROR!!!");
    #define dbgFlushType(a)

#else  // !NVSTATDRIVER && !DEBUG

    #define dbgD3DError()
    #define dbgFlushType(a)

#endif  // !NVSTATDRIVER && !DEBUG


#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _NV3DDBG_H_

