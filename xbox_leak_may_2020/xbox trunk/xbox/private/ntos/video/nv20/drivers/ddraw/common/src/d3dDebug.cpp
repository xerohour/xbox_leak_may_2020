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
//  Module: d3dDebug.cpp
//      D3D-specific debugging routines
//
// **************************************************************************
//
//  History:
//      Chas Inman              18Nov97         original version
//      Craig Duttweiler        21Nov00         major rehash
//
// **************************************************************************
#include "nvprecomp.h"

#if (NVARCH >= 0x04)

#ifdef WINNT
    void atexit (void) {};
#endif

//-------------------------------------------------------------------------
// GLOBAL VARIABLES
//---------------------------------------------------------------------------

#ifdef DEBUG

    // general debugging
    DWORD          dbgShowState             = 0x00000000;
    DWORD          dbgDumpState             = 0x00000000;
    DWORD          dbgFlush                 = 0x00000000; // Flush Nothing
    DWORD          dbgForceHWRefresh        = 0x00000000;

    // primary info (shared by front buffer rendering and texture previewing)
    DWORD          dbgPrimaryPitch          = 0;
    DWORD          dbgPrimaryHeight         = 0;
    DWORD          dbgPrimaryBPP            = 0;

    // front buffer rendering
    CSimpleSurface dbgFrontRenderSurface;
    BOOL           dbgFrontRender           = 0;

    // texture previewing
    DWORD          dbgTexturePreview        = 0;

#endif  // DEBUG

//-------------------------------------------------------------------------
// STRING TABLES
//---------------------------------------------------------------------------

#ifdef DEBUG

char szD3DBoolean[][32] =
{
    "FALSE",
    "TRUE",
};

char szD3DShadeMode[][32] =
{
    "NULL",
    "D3DSHADE_FLAT",
    "D3DSHADE_GOURAUD",
    "D3DSHADE_PHONG"
};

char szD3DFillMode[][32] =
{
    "NULL",
    "D3DFILL_POINT",
    "D3DFILL_WIREFRAME",
    "D3DFILL_SOLID"
};

char szD3DTextureFilter[][32] =
{
    "NULL",
    "D3DFILTER_NEAREST",
    "D3DFILTER_LINEAR",
    "D3DFILTER_MIPNEAREST",
    "D3DFILTER_MIPLINEAR",
    "D3DFILTER_LINEARMIPNEAREST",
    "D3DFILTER_LINEARMIPLINEAR"
};

char szD3DBlend[][32] =
{
    "NULL",
    "D3DBLEND_ZERO",
    "D3DBLEND_ONE",
    "D3DBLEND_SRCCOLOR",
    "D3DBLEND_INVSRCCOLOR",
    "D3DBLEND_SRCALPHA",
    "D3DBLEND_INVSRCALPHA",
    "D3DBLEND_DESTALPHA",
    "D3DBLEND_INVDESTALPHA",
    "D3DBLEND_DESTCOLOR",
    "D3DBLEND_INVDESTCOLOR",
    "D3DBLEND_SRCALPHASAT",
    "D3DBLEND_BOTHSRCALPHA",
    "D3DBLEND_BOTHINVSRCALPHA"
};

char szD3DTextureBlend[][32] =
{
    "NULL",
    "D3DTBLEND_DECAL",
    "D3DTBLEND_MODULATE",
    "D3DTBLEND_DECALALPHA",
    "D3DTBLEND_MODULATEALPHA",
    "D3DTBLEND_DECALMASK",
    "D3DTBLEND_MODULATEMASK",
    "D3DTBLEND_COPY",
    "D3DTBLEND_ADD"
};

char szD3DTextureAddress[][32] =
{
    "NULL",
    "D3DTADDRESS_WRAP",
    "D3DTADDRESS_MIRROR",
    "D3DTADDRESS_CLAMP",
    "D3DTADDRESS_BORDER"
};

char szD3DCull[][32] =
{
    "NULL",
    "D3DCULL_NONE",
    "D3DCULL_CW",
    "D3DCULL_CCW"
};

char szD3DCmpFunc[][32] =
{
    "NULL",
    "D3DCMP_NEVER",
    "D3DCMP_LESS",
    "D3DCMP_EQUAL",
    "D3DCMP_LESSEQUAL",
    "D3DCMP_GREATER",
    "D3DCMP_NOTEQUAL",
    "D3DCMP_GREATEREQUAL",
    "D3DCMP_ALWAYS"
};

char szD3DStencilOp[][32] =
{
    "NULL",
    "D3DSTENCILOP_KEEP",
    "D3DSTENCILOP_ZERO",
    "D3DSTENCILOP_REPLACE",
    "D3DSTENCILOP_INCRSAT",
    "D3DSTENCILOP_DECRSAT",
    "D3DSTENCILOP_INVERT",
    "D3DSTENCILOP_INCR",
    "D3DSTENCILOP_DECR"
};

char szD3DFogMode[][32] =
{
    "D3DFOG_NONE",
    "D3DFOG_EXP",
    "D3DFOG_EXP2",
    "D3DFOG_LINEAR"
};

char szD3DZBufferType[][32] =
{
    "D3DZB_FALSE",
    "D3DZB_TRUE",
    "D3DZB_USEW"
};

char szD3DAntiAliasMode[][32] =
{
    "NULL",
    "D3DANTIALIAS_NONE",
    "D3DANTIALIAS_SORTDEPENDENT",
    "D3DANTIALIAS_SORTINDEPENDENT"
};

char szD3DTextureOp[][36] =
{
    "NULL",
    "D3DTOP_DISABLE",
    "D3DTOP_SELECTARG1",
    "D3DTOP_SELECTARG2",
    "D3DTOP_MODULATE",
    "D3DTOP_MODULATE2X",
    "D3DTOP_MODULATE4X",
    "D3DTOP_ADD",
    "D3DTOP_ADDSIGNED",
    "D3DTOP_ADDSIGNED2X",
    "D3DTOP_SUBTRACT",
    "D3DTOP_ADDSMOOTH",
    "D3DTOP_BLENDDIFFUSEALPHA",
    "D3DTOP_BLENDTEXTUREALPHA",
    "D3DTOP_BLENDFACTORALPHA",
    "D3DTOP_BLENDTEXTUREALPHAPM",
    "D3DTOP_BLENDCURRENTALPHA",
    "D3DTOP_PREMODULATE",
    "D3DTOP_MODULATEALPHA_ADDCOLOR",
    "D3DTOP_MODULATECOLOR_ADDALPHA",
    "D3DTOP_MODULATEINVALPHA_ADDCOLOR",
    "D3DTOP_MODULATEINVCOLOR_ADDALPHA",
    "D3DTOP_BUMPENVMAP",
    "D3DTOP_BUMPENVMAPLUMINANCE",
    "D3DTOP_DOTPRODUCT3"
};

char szD3DArg[][64] =
{
    "D3DTA_DIFFUSE",
    "D3DTA_CURRENT",
    "D3DTA_TEXTURE",
    "D3DTA_TFACTOR",
    "INVALID",
    "INVALID",
    "INVALID",
    "INVALID",
    "INVALID",
    "INVALID",
    "INVALID",
    "INVALID",
    "INVALID",
    "INVALID",
    "INVALID",
    "INVALID",
    "D3DTA_DIFFUSE | D3DTA_COMPLEMENT",
    "D3DTA_CURRENT | D3DTA_COMPLEMENT",
    "D3DTA_TEXTURE | D3DTA_COMPLEMENT",
    "D3DTA_TFACTOR | D3DTA_COMPLEMENT",
    "INVALID",
    "INVALID",
    "INVALID",
    "INVALID",
    "INVALID",
    "INVALID",
    "INVALID",
    "INVALID",
    "INVALID",
    "INVALID",
    "INVALID",
    "INVALID",
    "D3DTA_DIFFUSE | D3DTA_ALPHAREPLICATE",
    "D3DTA_CURRENT | D3DTA_ALPHAREPLICATE",
    "D3DTA_TEXTURE | D3DTA_ALPHAREPLICATE",
    "D3DTA_TFACTOR | D3DTA_ALPHAREPLICATE",
    "INVALID",
    "INVALID",
    "INVALID",
    "INVALID",
    "INVALID",
    "INVALID",
    "INVALID",
    "INVALID",
    "INVALID",
    "INVALID",
    "INVALID",
    "INVALID",
    "D3DTA_DIFFUSE | D3DTA_COMPLEMENT | D3DTA_ALPHAREPLICATE",
    "D3DTA_CURRENT | D3DTA_COMPLEMENT | D3DTA_ALPHAREPLICATE",
    "D3DTA_TEXTURE | D3DTA_COMPLEMENT | D3DTA_ALPHAREPLICATE",
    "D3DTA_TFACTOR | D3DTA_COMPLEMENT | D3DTA_ALPHAREPLICATE",
    "INVALID",
    "INVALID",
    "INVALID",
    "INVALID",
    "INVALID",
    "INVALID",
    "INVALID",
    "INVALID",
    "INVALID",
    "INVALID",
    "INVALID",
    "INVALID"
};

char szD3DArgModifier[][32] =
{
    "D3DTA_NORMAL",
    "D3DTA_COMPLEMENT",
    "D3DTA_ALPHAREPLICATE"
};

char szD3DTextureMagFilter[][32] =
{
    "NULL",
    "D3DTFG_POINT",
    "D3DTFG_LINEAR",
    "D3DTFG_FLATCUBIC",
    "D3DTFG_GAUSSIANCUBIC",
    "D3DTFG_ANISOTROPIC"
};

char szD3DTextureMinFilter[][32] =
{
    "NULL",
    "D3DTFN_POINT",
    "D3DTFN_LINEAR",
    "D3DTFN_ANISOTROPIC"
};

char szD3DTextureMipFilter[][32] =
{
    "NULL",
    "D3DTFP_POINT",
    "D3DTFP_LINEAR"
};

char szD3DRenderStateLookupTable[D3D_RENDERSTATE_MAX+1][50] =
{
    "D3DRENDERSTATE_UNKNOWN",                       // 0
    "D3DRENDERSTATE_TEXTUREHANDLE",                 // 1
    "D3DRENDERSTATE_ANTIALIAS",                     // 2   D3DANTIALIASMODE
    "D3DRENDERSTATE_TEXTUREADDRESS",                // 3
    "D3DRENDERSTATE_TEXTUREPERSPECTIVE",            // 4   TRUE for perspective correction
    "D3DRENDERSTATE_WRAPU",                         // 5
    "D3DRENDERSTATE_WRAPV",                         // 6
    "D3DRENDERSTATE_ZENABLE",                       // 7   D3DZBUFFERTYPE (or TRUE/FALSE for legacy)
    "D3DRENDERSTATE_FILLMODE",                      // 8   D3DFILL_MODE
    "D3DRENDERSTATE_SHADEMODE",                     // 9   D3DSHADEMODE
    "D3DRENDERSTATE_LINEPATTERN",                   // 10  D3DLINEPATTERN
    "D3DRENDERSTATE_MONOENABLE",                    // 11  TRUE to enable mono rasterization
    "D3DRENDERSTATE_ROP2",                          // 12  ROP2
    "D3DRENDERSTATE_PLANEMASK",                     // 13
    "D3DRENDERSTATE_ZWRITEENABLE",                  // 14  TRUE to enable z writes
    "D3DRENDERSTATE_ALPHATESTENABLE",               // 15  TRUE to enable alpha tests
    "D3DRENDERSTATE_LASTPIXEL",                     // 16  TRUE for last-pixel on lines
    "D3DRENDERSTATE_TEXTUREMAG",                    // 17
    "D3DRENDERSTATE_TEXTUREMIN",                    // 18
    "D3DRENDERSTATE_SRCBLEND",                      // 19  D3DBLEND
    "D3DRENDERSTATE_DESTBLEND",                     // 20  D3DBLEND
    "D3DRENDERSTATE_TEXTUREMAPBLEND",               // 21
    "D3DRENDERSTATE_CULLMODE",                      // 22  D3DCULL
    "D3DRENDERSTATE_ZFUNC",                         // 23  D3DCMPFUNC
    "D3DRENDERSTATE_ALPHAREF",                      // 24  D3DFIXED
    "D3DRENDERSTATE_ALPHAFUNC",                     // 25  D3DCMPFUNC
    "D3DRENDERSTATE_DITHERENABLE",                  // 26  TRUE to enable dithering
    "D3DRENDERSTATE_ALPHABLENDENABLE",              // 27  TRUE to enable alpha blending
    "D3DRENDERSTATE_FOGENABLE",                     // 28  TRUE to enable fog blending
    "D3DRENDERSTATE_SPECULARENABLE",                // 29  TRUE to enable specular
    "D3DRENDERSTATE_ZVISIBLE",                      // 30  TRUE to enable z checking
    "D3DRENDERSTATE_SUBPIXEL",                      // 31
    "D3DRENDERSTATE_SUBPIXELX",                     // 32
    "D3DRENDERSTATE_STIPPLEDALPHA",                 // 33  TRUE to enable stippled alpha (RGB device only)
    "D3DRENDERSTATE_FOGCOLOR",                      // 34  D3DCOLOR
    "D3DRENDERSTATE_FOGTABLEMODE",                  // 35  D3DFOGMODE
    "D3DRENDERSTATE_FOGSTART",                      // 36  Fog start (for both vertex and pixel fog)
    "D3DRENDERSTATE_FOGEND",                        // 37  Fog end
    "D3DRENDERSTATE_FOGDENSITY",                    // 38  Fog density
    "D3DRENDERSTATE_STIPPLEENABLE",                 // 39  TRUE to enable stippling
    "D3DRENDERSTATE_EDGEANTIALIAS",                 // 40  TRUE to enable edge antialiasing
    "D3DRENDERSTATE_COLORKEYENABLE",                // 41  TRUE to enable source colorkeyed textures
    "D3DRENDERSTATE_UNKNOWN",                       // 42
    "D3DRENDERSTATE_BORDERCOLOR",                   // 43  Border color for texturing w/border
    "D3DRENDERSTATE_TEXTUREADDRESSU",               // 44  Texture addressing mode for U coordinate
    "D3DRENDERSTATE_TEXTUREADDRESSV",               // 45  Texture addressing mode for V coordinate
    "D3DRENDERSTATE_MIPMAPLODBIAS",                 // 46  D3DVALUE Mipmap LOD bias
    "D3DRENDERSTATE_ZBIAS",                         // 47  LONG Z bias
    "D3DRENDERSTATE_RANGEFOGENABLE",                // 48  Enables range-based fog
    "D3DRENDERSTATE_ANISOTROPY",                    // 49  Max. anisotropy. 1 = no anisotropy
    "D3DRENDERSTATE_FLUSHBATCH",                    // 50  Explicit flush for DP batching (DX5 Only)
    "D3DRENDERSTATE_TRANSLUCENTSORTINDEPENDENT",    // 51  BOOL enable sort-independent transparency
    "D3DRENDERSTATE_STENCILENABLE",                 // 52  BOOL enable/disable stenciling
    "D3DRENDERSTATE_STENCILFAIL",                   // 53  D3DSTENCILOP to do if stencil test fails
    "D3DRENDERSTATE_STENCILZFAIL",                  // 54  D3DSTENCILOP to do if stencil test passes and Z test fails
    "D3DRENDERSTATE_STENCILPASS",                   // 55  D3DSTENCILOP to do if both stencil and Z tests pass
    "D3DRENDERSTATE_STENCILFUNC",                   // 56  D3DCMPFUNC fn.  Stencil Test passes if ((ref & mask) stencilfn (stencil & mask)) is true
    "D3DRENDERSTATE_STENCILREF",                    // 57  Reference value used in stencil test
    "D3DRENDERSTATE_STENCILMASK",                   // 58  Mask value used in stencil test
    "D3DRENDERSTATE_STENCILWRITEMASK",              // 59  Write mask applied to values written to stencil buffer
    "D3DRENDERSTATE_TEXTUREFACTOR",                 // 60  D3DCOLOR used for multi-texture blend
    "D3DRENDERSTATE_EVICTMANAGEDTEXTURES",          // 61  D3DRENDERSTATE_EVICTMANAGEDTEXTURES???
    "D3DRENDERSTATE_SCENECAPTURE",                  // 62  D3DRENDERSTATE_SCENECAPTURE???
    "D3DRENDERSTATE_UNKNOWN",                       // 63  invalid???
    "D3DRENDERSTATE_STIPPLEPATTERN00",              // 64  Stipple pattern 01...
    "D3DRENDERSTATE_STIPPLEPATTERN01",              // 65
    "D3DRENDERSTATE_STIPPLEPATTERN02",              // 66
    "D3DRENDERSTATE_STIPPLEPATTERN03",              // 67
    "D3DRENDERSTATE_STIPPLEPATTERN04",              // 68
    "D3DRENDERSTATE_STIPPLEPATTERN05",              // 69
    "D3DRENDERSTATE_STIPPLEPATTERN06",              // 70
    "D3DRENDERSTATE_STIPPLEPATTERN07",              // 71
    "D3DRENDERSTATE_STIPPLEPATTERN08",              // 72
    "D3DRENDERSTATE_STIPPLEPATTERN09",              // 73
    "D3DRENDERSTATE_STIPPLEPATTERN10",              // 74
    "D3DRENDERSTATE_STIPPLEPATTERN11",              // 75
    "D3DRENDERSTATE_STIPPLEPATTERN12",              // 76
    "D3DRENDERSTATE_STIPPLEPATTERN13",              // 77
    "D3DRENDERSTATE_STIPPLEPATTERN14",              // 78
    "D3DRENDERSTATE_STIPPLEPATTERN15",              // 79
    "D3DRENDERSTATE_STIPPLEPATTERN16",              // 80
    "D3DRENDERSTATE_STIPPLEPATTERN17",              // 81
    "D3DRENDERSTATE_STIPPLEPATTERN18",              // 82
    "D3DRENDERSTATE_STIPPLEPATTERN19",              // 83
    "D3DRENDERSTATE_STIPPLEPATTERN20",              // 84
    "D3DRENDERSTATE_STIPPLEPATTERN21",              // 85
    "D3DRENDERSTATE_STIPPLEPATTERN22",              // 86
    "D3DRENDERSTATE_STIPPLEPATTERN23",              // 87
    "D3DRENDERSTATE_STIPPLEPATTERN24",              // 88
    "D3DRENDERSTATE_STIPPLEPATTERN25",              // 89
    "D3DRENDERSTATE_STIPPLEPATTERN26",              // 90
    "D3DRENDERSTATE_STIPPLEPATTERN27",              // 91
    "D3DRENDERSTATE_STIPPLEPATTERN28",              // 92
    "D3DRENDERSTATE_STIPPLEPATTERN29",              // 93
    "D3DRENDERSTATE_STIPPLEPATTERN30",              // 94
    "D3DRENDERSTATE_STIPPLEPATTERN31",              // 95
    "D3DRENDERSTATE_UNKNOWN",                       // 96
    "D3DRENDERSTATE_UNKNOWN",                       // 97
    "D3DRENDERSTATE_UNKNOWN",                       // 98
    "D3DRENDERSTATE_UNKNOWN",                       // 99
    "D3DRENDERSTATE_UNKNOWN",                       // 100
    "D3DRENDERSTATE_UNKNOWN",                       // 101
    "D3DRENDERSTATE_UNKNOWN",                       // 102
    "D3DRENDERSTATE_UNKNOWN",                       // 103
    "D3DRENDERSTATE_UNKNOWN",                       // 104
    "D3DRENDERSTATE_UNKNOWN",                       // 105
    "D3DRENDERSTATE_UNKNOWN",                       // 106
    "D3DRENDERSTATE_UNKNOWN",                       // 107
    "D3DRENDERSTATE_UNKNOWN",                       // 108
    "D3DRENDERSTATE_UNKNOWN",                       // 109
    "D3DRENDERSTATE_UNKNOWN",                       // 110
    "D3DRENDERSTATE_UNKNOWN",                       // 111
    "D3DRENDERSTATE_UNKNOWN",                       // 112
    "D3DRENDERSTATE_UNKNOWN",                       // 113
    "D3DRENDERSTATE_UNKNOWN",                       // 114
    "D3DRENDERSTATE_UNKNOWN",                       // 115
    "D3DRENDERSTATE_UNKNOWN",                       // 116
    "D3DRENDERSTATE_UNKNOWN",                       // 117
    "D3DRENDERSTATE_UNKNOWN",                       // 118
    "D3DRENDERSTATE_UNKNOWN",                       // 119
    "D3DRENDERSTATE_UNKNOWN",                       // 120
    "D3DRENDERSTATE_UNKNOWN",                       // 121
    "D3DRENDERSTATE_UNKNOWN",                       // 122
    "D3DRENDERSTATE_UNKNOWN",                       // 123
    "D3DRENDERSTATE_UNKNOWN",                       // 124
    "D3DRENDERSTATE_UNKNOWN",                       // 125
    "D3DRENDERSTATE_UNKNOWN",                       // 126
    "D3DRENDERSTATE_UNKNOWN",                       // 127
    "D3DRENDERSTATE_WRAP0",                         // 128  wrap for 1st texture coord. set
    "D3DRENDERSTATE_WRAP1",                         // 129  wrap for 2nd texture coord. set
    "D3DRENDERSTATE_WRAP2",                         // 130  wrap for 3rd texture coord. set
    "D3DRENDERSTATE_WRAP3",                         // 131  wrap for 4th texture coord. set
    "D3DRENDERSTATE_WRAP4",                         // 132  wrap for 5th texture coord. set
    "D3DRENDERSTATE_WRAP5",                         // 133  wrap for 6th texture coord. set
    "D3DRENDERSTATE_WRAP6",                         // 134  wrap for 7th texture coord. set
    "D3DRENDERSTATE_WRAP7",                         // 135  wrap for 8th texture coord. set
    "D3DRENDERSTATE_CLIPPING",                      // 136
    "D3DRENDERSTATE_LIGHTING",                      // 137
    "D3DRENDERSTATE_EXTENTS",                       // 138
    "D3DRENDERSTATE_AMBIENT",                       // 139
    "D3DRENDERSTATE_FOGVERTEXMODE",                 // 140
    "D3DRENDERSTATE_COLORVERTEX",                   // 141
    "D3DRENDERSTATE_LOCALVIEWER",                   // 142
    "D3DRENDERSTATE_NORMALIZENORMALS",              // 143
    "D3DRENDERSTATE_COLORKEYBLENDENABLE",           // 144
    "D3DRENDERSTATE_DIFFUSEMATERIALSOURCE",         // 145
    "D3DRENDERSTATE_SPECULARMATERIALSOURCE",        // 146
    "D3DRENDERSTATE_AMBIENTMATERIALSOURCE",         // 147
    "D3DRENDERSTATE_EMISSIVEMATERIALSOURCE",        // 148
    "D3DRENDERSTATE_UNKNOWN",                       // 149
    "D3DRENDERSTATE_UNKNOWN",                       // 150
    "D3DRENDERSTATE_VERTEXBLEND",                   // 151
    "D3DRENDERSTATE_CLIPPLANEENABLE",               // 152
    "D3DRS_SOFTWAREVERTEXPROCESSING",               // 153
    "D3DRS_POINTSIZE",                              // 154
    "D3DRS_POINTSIZE_MIN",                          // 155
    "D3DRS_POINTSPRITEENABLE",                      // 156
    "D3DRS_POINTSCALEENABLE",                       // 157
    "D3DRS_POINTSCALE_A",                           // 158
    "D3DRS_POINTSCALE_B",                           // 159
    "D3DRS_POINTSCALE_C",                           // 160
    "D3DRS_MULTISAMPLEANTIALIAS",                   // 161
    "D3DRS_MULTISAMPLEMASK",                        // 162
    "D3DRS_PATCHEDGESTYLE",                         // 163
    "D3DRS_PRIMSEGMENTS",                           // 164
    "D3DRS_DEBUGMONITORTOKEN",                      // 165
    "D3DRS_POINTSIZE_MAX",                          // 166
    "D3DRS_INDEXEDVERTEXBLENDENABLE",               // 167
    "D3DRS_COLORWRITEENABLE",                       // 168
    "D3DRENDERSTATE_UNKNOWN",                       // 169
    "D3DRS_TWEENFACTOR",                            // 170
    "D3DRS_BLENDOP"                                 // 171
};

char szD3DDP2OpLookupTable[D3D_DP2OP_MAX+1][64] =
{
    "D3DDP2OP_UNKNOWN",                             //  0
    "D3DDP2OP_POINTS",                              //  1
    "D3DDP2OP_INDEXEDLINELIST",                     //  2
    "D3DDP2OP_INDEXEDTRIANGLELIST",                 //  3
    "D3DDP2OP_RESERVED0",                           //  4
    "D3DDP2OP_UNKNOWN",                             //  5
    "D3DDP2OP_UNKNOWN",                             //  6
    "D3DDP2OP_UNKNOWN",                             //  7
    "D3DDP2OP_RENDERSTATE",                         //  8
    "D3DDP2OP_UNKNOWN",                             //  9
    "D3DDP2OP_UNKNOWN",                             // 10
    "D3DDP2OP_END",                                 // 11
    "D3DDP2OP_UNKNOWN",                             // 12
    "D3DDP2OP_UNKNOWN",                             // 13
    "D3DDP2OP_UNKNOWN",                             // 14
    "D3DDP2OP_LINELIST",                            // 15
    "D3DDP2OP_LINESTRIP",                           // 16
    "D3DDP2OP_INDEXEDLINESTRIP",                    // 17
    "D3DDP2OP_TRIANGLELIST",                        // 18
    "D3DDP2OP_TRIANGLESTRIP",                       // 19
    "D3DDP2OP_INDEXEDTRIANGLESTRIP",                // 20
    "D3DDP2OP_TRIANGLEFAN",                         // 21
    "D3DDP2OP_INDEXEDTRIANGLEFAN",                  // 22
    "D3DDP2OP_TRIANGLEFAN_IMM",                     // 23
    "D3DDP2OP_LINELIST_IMM",                        // 24
    "D3DDP2OP_TEXTURESTAGESTATE",                   // 25
    "D3DDP2OP_INDEXEDTRIANGLELIST2",                // 26
    "D3DDP2OP_INDEXEDLINELIST2",                    // 27
    "D3DDP2OP_VIEWPORTINFO",                        // 28
    "D3DDP2OP_WINFO",                               // 29
    "D3DDP2OP_SETPALETTE",                          // 30
    "D3DDP2OP_UPDATEPALETTE",                       // 31
    "D3DDP2OP_ZRANGE",                              // 32
    "D3DDP2OP_SETMATERIAL",                         // 33
    "D3DDP2OP_SETLIGHT",                            // 34
    "D3DDP2OP_CREATELIGHT",                         // 35
    "D3DDP2OP_SETTRANSFORM",                        // 36
    "D3DDP2OP_EXT",                                 // 37
    "D3DDP2OP_TEXBLT",                              // 38
    "D3DDP2OP_STATESET",                            // 39
    "D3DDP2OP_SETPRIORITY",                         // 40
    "D3DDP2OP_SETRENDERTARGET",                     // 41
    "D3DDP2OP_CLEAR",                               // 42
    "D3DDP2OP_SETTEXLOD",                           // 43
    "D3DDP2OP_SETCLIPPLANE",                        // 44
    "D3DDP2OP_CREATEVERTEXSHADER",                  // 45
    "D3DDP2OP_DELETEVERTEXSHADER",                  // 46
    "D3DDP2OP_SETVERTEXSHADER",                     // 47
    "D3DDP2OP_SETVERTEXSHADERCONST",                // 48
    "D3DDP2OP_SETSTREAMSOURCE",                     // 49
    "D3DDP2OP_SETSTREAMSOURCEUM",                   // 50
    "D3DDP2OP_SETINDICES",                          // 51
    "D3DDP2OP_DRAWPRIMITIVE",                       // 52
    "D3DDP2OP_DRAWINDEXEDPRIMITIVE",                // 53
    "D3DDP2OP_CREATEPIXELSHADER",                   // 54
    "D3DDP2OP_DELETEPIXELSHADER",                   // 55
    "D3DDP2OP_SETPIXELSHADER",                      // 56
    "D3DDP2OP_SETPIXELSHADERCONST",                 // 57
    "D3DDP2OP_CLIPPEDTRIANGLEFAN",                  // 58
    "D3DDP2OP_DRAWPRIMITIVE2",                      // 59
    "D3DDP2OP_DRAWINDEXEDPRIMITIVE2",               // 60
    "D3DDP2OP_DRAWRECTSURFACE",                     // 61
    "D3DDP2OP_DRAWTRISURFACE",                      // 62
    "D3DDP2OP_VOLUMEBLT",                           // 63
    "D3DDP2OP_BUFFERBLT",                           // 64
    "D3DDP2OP_MULTIPLYTRANSFORM"                    // 65
};

#endif  // DEBUG

//-------------------------------------------------------------------------
// DEBUG PRINTING
//---------------------------------------------------------------------------

#ifdef DEBUG

// conditionally print a string out as debug message and/or to a file

void __cdecl dbgDPFAndDump (DWORD flag, LPSTR szFormat, ...)
{
    char str[256];

    nvSprintfVAL (str, szFormat, (va_list)(&szFormat+1));

    if ((flag == NVDBG_SHOW_MANUAL_OVERRIDE) || (dbgShowState & flag)) {
        DPF("%s", str);
    }

    if ((flag == NVDBG_SHOW_MANUAL_OVERRIDE) || (dbgDumpState & flag)) {
        char  *filename;
        HANDLE hFile;
        DWORD  dwJunk;

        // find the appropriate filename
        switch (flag) {
            case NVDBG_SHOW_RENDER_STATE:
                filename = NVDBG_DUMP_RENDER_STATE_FILENAME;
                break;
            case NVDBG_SHOW_DX5_CLASS_STATE:
                filename = NVDBG_DUMP_DX5_CLASS_STATE_FILENAME;
                break;
            case NVDBG_SHOW_SURFACE_STATE:
                filename = NVDBG_DUMP_SURFACE_STATE_FILENAME;
                break;
            case NVDBG_SHOW_VERTICES:
                filename = NVDBG_DUMP_VERTICES_FILENAME;
                break;
            case NVDBG_SHOW_MTS_STATE:
                filename = NVDBG_DUMP_MTS_STATE_FILENAME;
                break;
            case NVDBG_SHOW_DX6_CLASS_STATE:
                filename = NVDBG_DUMP_DX6_CLASS_STATE_FILENAME;
                break;
            case NVDBG_SHOW_DRAWPRIM2_INFO:
                filename = NVDBG_DUMP_DRAWPRIM2_INFO_FILENAME;
                break;
            case NVDBG_SHOW_MANUAL_OVERRIDE:
                filename = NVDBG_DUMP_MANUAL_OVERRIDE_FILENAME;
                break;
            default:
                DPF("dbgDPFAndDump: unknown flag");
                return;
                break;
        }

        // open the file if it exists, create it if not.
        hFile = NvCreateFile(filename,GENERIC_WRITE,0,0,OPEN_ALWAYS,0,0);
        if (hFile == INVALID_HANDLE_VALUE) {
            DPF ("dbgDPFAndDump: File open failed");
            __asm int 3;
        }

        // set the file pointer to the end. this appears to be
        // the only way to append to a file. how retarded.
        SetFilePointer(hFile, 0,0, FILE_END);

        NvWriteFile(hFile,str,nvStrLen(str),&dwJunk,0);
        NvWriteFile(hFile,"\r\n",2,&dwJunk,0);
        FlushFileBuffers(hFile);
        NvCloseHandle(hFile);
    }

}

//-------------------------------------------------------------------------
// STATE DUMPING
//-------------------------------------------------------------------------

void dbgDisplayContextState
(
    DWORD           dwFlags,
    PNVD3DCONTEXT   pContext
)
{
    BOOL  bDPF, bDump;
    DWORD flag;

    if (!(dbgShowState || dbgDumpState)) return;

    bDPF = dbgShowState & NVDBG_SHOW_RENDER_STATE;
    bDump = dbgDumpState & NVDBG_SHOW_RENDER_STATE;
    flag = NVDBG_SHOW_RENDER_STATE;

    if ((bDPF || bDump) &&
        (dwFlags & NVDBG_SHOW_RENDER_STATE)) {
        dbgDPFAndDump(flag, "D3D:Context Render State:");
        dbgDPFAndDump(flag, "    TextureHandle *    = %08x", pContext->dwRenderState[D3DRENDERSTATE_TEXTUREHANDLE]);
        dbgDPFAndDump(flag, "    AntiAlias          = %s",   szD3DBoolean[pContext->dwRenderState[D3DRENDERSTATE_ANTIALIAS]]);
        dbgDPFAndDump(flag, "    TextureAddress *   = %s",   szD3DTextureAddress[pContext->dwRenderState[D3DRENDERSTATE_TEXTUREADDRESS]]);
        dbgDPFAndDump(flag, "    TexturePerspective = %s",   szD3DBoolean[pContext->dwRenderState[D3DRENDERSTATE_TEXTUREPERSPECTIVE]]);
        dbgDPFAndDump(flag, "    WrapU              = %s",   szD3DBoolean[pContext->dwRenderState[D3DRENDERSTATE_WRAPU]]);
        dbgDPFAndDump(flag, "    WrapV              = %s",   szD3DBoolean[pContext->dwRenderState[D3DRENDERSTATE_WRAPV]]);
        dbgDPFAndDump(flag, "    ZEnable            = %s",   szD3DZBufferType[pContext->dwRenderState[D3DRENDERSTATE_ZENABLE]]);
        dbgDPFAndDump(flag, "    FillMode           = %s",   szD3DFillMode[pContext->dwRenderState[D3DRENDERSTATE_FILLMODE]]);
        dbgDPFAndDump(flag, "    ShadeMode          = %s",   szD3DShadeMode[pContext->dwRenderState[D3DRENDERSTATE_SHADEMODE]]);
        dbgDPFAndDump(flag, "    LinePattern        = %08x", pContext->dwRenderState[D3DRENDERSTATE_LINEPATTERN]);
        dbgDPFAndDump(flag, "    MonoEnable         = %08x", pContext->dwRenderState[D3DRENDERSTATE_MONOENABLE]);
        dbgDPFAndDump(flag, "    Rop2               = %08x", pContext->dwRenderState[D3DRENDERSTATE_ROP2]);
        dbgDPFAndDump(flag, "    PlaneMask          = %08x", pContext->dwRenderState[D3DRENDERSTATE_PLANEMASK]);
        dbgDPFAndDump(flag, "    ZWriteEnable       = %s",   szD3DBoolean[pContext->dwRenderState[D3DRENDERSTATE_ZWRITEENABLE]]);
        dbgDPFAndDump(flag, "    AlphaTestEnable    = %s",   szD3DBoolean[pContext->dwRenderState[D3DRENDERSTATE_ALPHATESTENABLE]]);
        dbgDPFAndDump(flag, "    LastPixelEnable    = %s",   szD3DBoolean[pContext->dwRenderState[D3DRENDERSTATE_LASTPIXEL]]);
        dbgDPFAndDump(flag, "    TextureMag         = %s",   szD3DTextureFilter[pContext->dwRenderState[D3DRENDERSTATE_TEXTUREMAG]]);
        dbgDPFAndDump(flag, "    TextureMin         = %s",   szD3DTextureFilter[pContext->dwRenderState[D3DRENDERSTATE_TEXTUREMIN]]);
        dbgDPFAndDump(flag, "    SrcBlend           = %s",   szD3DBlend[pContext->dwRenderState[D3DRENDERSTATE_SRCBLEND]]);
        dbgDPFAndDump(flag, "    DstBlend           = %s",   szD3DBlend[pContext->dwRenderState[D3DRENDERSTATE_DESTBLEND]]);
        dbgDPFAndDump(flag, "    TextureMapBlend    = %s",   szD3DTextureBlend[pContext->dwRenderState[D3DRENDERSTATE_TEXTUREMAPBLEND]]);
        dbgDPFAndDump(flag, "    CullMode           = %s",   szD3DCull[pContext->dwRenderState[D3DRENDERSTATE_CULLMODE]]);
        dbgDPFAndDump(flag, "    ZFunc              = %s",   szD3DCmpFunc[pContext->dwRenderState[D3DRENDERSTATE_ZFUNC]]);
        dbgDPFAndDump(flag, "    AlphaRef           = %08x", pContext->dwRenderState[D3DRENDERSTATE_ALPHAREF]);
        dbgDPFAndDump(flag, "    AlphaFunc          = %s",   szD3DCmpFunc[pContext->dwRenderState[D3DRENDERSTATE_ALPHAFUNC]]);
        dbgDPFAndDump(flag, "    DitherEnable       = %s",   szD3DBoolean[pContext->dwRenderState[D3DRENDERSTATE_DITHERENABLE]]);
        dbgDPFAndDump(flag, "    AlphaBlendEnable   = %s",   szD3DBoolean[pContext->dwRenderState[D3DRENDERSTATE_ALPHABLENDENABLE]]);
        dbgDPFAndDump(flag, "    FogEnable          = %s",   szD3DBoolean[pContext->dwRenderState[D3DRENDERSTATE_FOGENABLE]]);
        dbgDPFAndDump(flag, "    SpecularEnable     = %s",   szD3DBoolean[pContext->dwRenderState[D3DRENDERSTATE_SPECULARENABLE]]);
        dbgDPFAndDump(flag, "    ZVisible           = %08x", pContext->dwRenderState[D3DRENDERSTATE_ZVISIBLE]);
        dbgDPFAndDump(flag, "    SubPixel           = %08x", pContext->dwRenderState[D3DRENDERSTATE_SUBPIXEL]);
        dbgDPFAndDump(flag, "    SubPixelX          = %08x", pContext->dwRenderState[D3DRENDERSTATE_SUBPIXELX]);
        dbgDPFAndDump(flag, "    StippledAlpha      = %08x", pContext->dwRenderState[D3DRENDERSTATE_STIPPLEDALPHA]);
        dbgDPFAndDump(flag, "    FogColor           = %08x", pContext->dwRenderState[D3DRENDERSTATE_FOGCOLOR]);
        dbgDPFAndDump(flag, "    FogTableMode       = %s",   szD3DFogMode[pContext->dwRenderState[D3DRENDERSTATE_FOGTABLEMODE]]);
        dbgDPFAndDump(flag, "    FogTableStart      = %f",   pContext->dwRenderState[D3DRENDERSTATE_FOGTABLESTART]);
        dbgDPFAndDump(flag, "    FogTableEnd        = %f",   pContext->dwRenderState[D3DRENDERSTATE_FOGTABLEEND]);
        dbgDPFAndDump(flag, "    FogTableDensity    = %f",   pContext->dwRenderState[D3DRENDERSTATE_FOGTABLEDENSITY]);
        dbgDPFAndDump(flag, "    StippledEnable     = %s",   szD3DBoolean[pContext->dwRenderState[D3DRENDERSTATE_STIPPLEENABLE]]);
        dbgDPFAndDump(flag, "    EdgeAntiAlias      = %s",   szD3DBoolean[pContext->dwRenderState[D3DRENDERSTATE_EDGEANTIALIAS]]);
        dbgDPFAndDump(flag, "    ColorKeyEnable     = %s",   szD3DBoolean[pContext->dwRenderState[D3DRENDERSTATE_COLORKEYENABLE]]);
        dbgDPFAndDump(flag, "    BorderColor        = %08x", pContext->dwRenderState[D3DRENDERSTATE_BORDERCOLOR]);
        dbgDPFAndDump(flag, "    TextureAddressU *  = %s",   szD3DTextureAddress[pContext->dwRenderState[D3DRENDERSTATE_TEXTUREADDRESSU]]);
        dbgDPFAndDump(flag, "    TextureAddressV *  = %s",   szD3DTextureAddress[pContext->dwRenderState[D3DRENDERSTATE_TEXTUREADDRESSV]]);
        dbgDPFAndDump(flag, "    MipMapLODBias *    = %08x", pContext->dwRenderState[D3DRENDERSTATE_MIPMAPLODBIAS]);
        dbgDPFAndDump(flag, "    ZBias              = %08x", pContext->dwRenderState[D3DRENDERSTATE_ZBIAS]);
        dbgDPFAndDump(flag, "    RangeFogEnable     = %s",   szD3DBoolean[pContext->dwRenderState[D3DRENDERSTATE_RANGEFOGENABLE]]);
        dbgDPFAndDump(flag, "    Anisotropy *       = %08x", pContext->dwRenderState[D3DRENDERSTATE_ANISOTROPY]);
        dbgDPFAndDump(flag, "    FlushBatch         = %08x", pContext->dwRenderState[D3DRENDERSTATE_FLUSHBATCH]);
        dbgDPFAndDump(flag, "    TranslucentSortInd = %08x", pContext->dwRenderState[D3DRENDERSTATE_TRANSLUCENTSORTINDEPENDENT]);
        dbgDPFAndDump(flag, "    StencilEnable      = %s",   szD3DBoolean[pContext->dwRenderState[D3DRENDERSTATE_STENCILENABLE]]);
        dbgDPFAndDump(flag, "    StencilFail        = %s",   szD3DStencilOp[pContext->dwRenderState[D3DRENDERSTATE_STENCILFAIL]]);
        dbgDPFAndDump(flag, "    StencilZFail       = %s",   szD3DStencilOp[pContext->dwRenderState[D3DRENDERSTATE_STENCILZFAIL]]);
        dbgDPFAndDump(flag, "    StencilPass        = %s",   szD3DStencilOp[pContext->dwRenderState[D3DRENDERSTATE_STENCILPASS]]);
        dbgDPFAndDump(flag, "    StencilFunc        = %s",   szD3DCmpFunc[pContext->dwRenderState[D3DRENDERSTATE_STENCILFUNC]]);
        dbgDPFAndDump(flag, "    StencilRef         = %08x", pContext->dwRenderState[D3DRENDERSTATE_STENCILREF]);
        dbgDPFAndDump(flag, "    StencilMask        = %08x", pContext->dwRenderState[D3DRENDERSTATE_STENCILMASK]);
        dbgDPFAndDump(flag, "    StencilWriteMask   = %08x", pContext->dwRenderState[D3DRENDERSTATE_STENCILWRITEMASK]);
        dbgDPFAndDump(flag, "    TextureFactor      = %08x", pContext->dwRenderState[D3DRENDERSTATE_TEXTUREFACTOR]);
        dbgDPFAndDump(flag, "    Wrap0              = %08x", pContext->dwRenderState[D3DRENDERSTATE_WRAP0]);
        dbgDPFAndDump(flag, "    Wrap1              = %08x", pContext->dwRenderState[D3DRENDERSTATE_WRAP1]);
        dbgDPFAndDump(flag, "    Wrap2              = %08x", pContext->dwRenderState[D3DRENDERSTATE_WRAP2]);
        dbgDPFAndDump(flag, "    Wrap3              = %08x", pContext->dwRenderState[D3DRENDERSTATE_WRAP3]);
        dbgDPFAndDump(flag, "    Wrap4              = %08x", pContext->dwRenderState[D3DRENDERSTATE_WRAP4]);
        dbgDPFAndDump(flag, "    Wrap5              = %08x", pContext->dwRenderState[D3DRENDERSTATE_WRAP5]);
        dbgDPFAndDump(flag, "    Wrap6              = %08x", pContext->dwRenderState[D3DRENDERSTATE_WRAP6]);
        dbgDPFAndDump(flag, "    Wrap7              = %08x", pContext->dwRenderState[D3DRENDERSTATE_WRAP7]);
        dbgDPFAndDump(flag, "");
        dbgDPFAndDump(flag, "* These fields may contain invalid junk. They have been superceded");
        dbgDPFAndDump(flag, "  by equivalent fields in tssState[0] and should never be referenced.");
        dbgDPFAndDump(flag, "");
    }

    bDPF = dbgShowState & NVDBG_SHOW_MTS_STATE;
    bDump = dbgDumpState & NVDBG_SHOW_MTS_STATE;
    flag = NVDBG_SHOW_MTS_STATE;

    if ((bDPF || bDump) &&
        (dwFlags & NVDBG_SHOW_MTS_STATE)) {
        DWORD dwStage, dwNumStages;

        dwNumStages = (dbgShowState & NVDBG_SHOW_MTS_STATE) >> NVDBG_SHOW_MTS_STATE_SHIFT;
        dwNumStages = (dwNumStages <= NVDBG_SHOW_MTS_STATE_MAX) ? dwNumStages : NVDBG_SHOW_MTS_STATE_MAX;

        for (dwStage = 0; dwStage < dwNumStages; dwStage++) {
            dbgDPFAndDump(flag, "D3D:Texture Stage %d State:", dwStage);
            dbgDPFAndDump(flag, "    TextureMap     = %08x", pContext->tssState[dwStage].dwValue[D3DTSS_TEXTUREMAP]);
            dbgDPFAndDump(flag, "    AddressU       = %s",   szD3DTextureAddress[pContext->tssState[dwStage].dwValue[D3DTSS_ADDRESSU]]);
            dbgDPFAndDump(flag, "    AddressV       = %s",   szD3DTextureAddress[pContext->tssState[dwStage].dwValue[D3DTSS_ADDRESSV]]);
            dbgDPFAndDump(flag, "    MagFilter      = %s",   szD3DTextureMagFilter[pContext->tssState[dwStage].dwValue[D3DTSS_MAGFILTER]]);
            dbgDPFAndDump(flag, "    MinFilter      = %s",   szD3DTextureMinFilter[pContext->tssState[dwStage].dwValue[D3DTSS_MINFILTER]]);
            dbgDPFAndDump(flag, "    MipFilter      = %s",   szD3DTextureMipFilter[pContext->tssState[dwStage].dwValue[D3DTSS_MIPFILTER]]);
            dbgDPFAndDump(flag, "    ColorOp        = %s",   szD3DTextureOp[pContext->tssState[dwStage].dwValue[D3DTSS_COLOROP]]);
            dbgDPFAndDump(flag, "    ColorArg1      = %s",   szD3DArg[pContext->tssState[dwStage].dwValue[D3DTSS_COLORARG1]]);
            dbgDPFAndDump(flag, "    ColorArg2      = %s",   szD3DArg[pContext->tssState[dwStage].dwValue[D3DTSS_COLORARG2]]);
            dbgDPFAndDump(flag, "    AlphaOp        = %s",   szD3DTextureOp[pContext->tssState[dwStage].dwValue[D3DTSS_ALPHAOP]]);
            dbgDPFAndDump(flag, "    AlphaArg1      = %s",   szD3DArg[pContext->tssState[dwStage].dwValue[D3DTSS_ALPHAARG1]]);
            dbgDPFAndDump(flag, "    AlphaArg2      = %s",   szD3DArg[pContext->tssState[dwStage].dwValue[D3DTSS_ALPHAARG2]]);
            dbgDPFAndDump(flag, "    BumpEnvMat00   = %08x", pContext->tssState[dwStage].dwValue[D3DTSS_BUMPENVMAT00]);
            dbgDPFAndDump(flag, "    BumpEnvMat01   = %08x", pContext->tssState[dwStage].dwValue[D3DTSS_BUMPENVMAT01]);
            dbgDPFAndDump(flag, "    BumpEnvMat10   = %08x", pContext->tssState[dwStage].dwValue[D3DTSS_BUMPENVMAT10]);
            dbgDPFAndDump(flag, "    BumpEnvMat11   = %08x", pContext->tssState[dwStage].dwValue[D3DTSS_BUMPENVMAT11]);
            dbgDPFAndDump(flag, "    TexCoordIndex  = %08x", pContext->tssState[dwStage].dwValue[D3DTSS_TEXCOORDINDEX]);
            dbgDPFAndDump(flag, "    BorderColor    = %08x", pContext->tssState[dwStage].dwValue[D3DTSS_BORDERCOLOR]);
            dbgDPFAndDump(flag, "    MipMapLODBias  = %08x", pContext->tssState[dwStage].dwValue[D3DTSS_MIPMAPLODBIAS]);
            dbgDPFAndDump(flag, "    MaxMipMapLevel = %08x", pContext->tssState[dwStage].dwValue[D3DTSS_MAXMIPLEVEL]);
            dbgDPFAndDump(flag, "    MaxAnisotropy  = %08x", pContext->tssState[dwStage].dwValue[D3DTSS_MAXANISOTROPY]);
            dbgDPFAndDump(flag, "    BumpEnvlScale  = %08x", pContext->tssState[dwStage].dwValue[D3DTSS_BUMPENVLSCALE]);
            dbgDPFAndDump(flag, "    BumpEnvlOffset = %08x", pContext->tssState[dwStage].dwValue[D3DTSS_BUMPENVLOFFSET]);
            dbgDPFAndDump(flag, "");
        }
    }

    bDPF = dbgShowState & NVDBG_SHOW_DX5_CLASS_STATE;
    bDump = dbgDumpState & NVDBG_SHOW_DX5_CLASS_STATE;
    flag = NVDBG_SHOW_DX5_CLASS_STATE;

    if ((bDPF || bDump) &&
        (dwFlags & NVDBG_SHOW_DX5_CLASS_STATE)) {
        dbgDPFAndDump(flag, "D3D:NV4 DX5 Class Context State:");
        dbgDPFAndDump(flag, "    ColorKey           = %08x", pContext->ctxInnerLoop.dwColorKey);
        dbgDPFAndDump(flag, "    TextureOffset      = %08x", pContext->ctxInnerLoop.dwTextureOffset);
        dbgDPFAndDump(flag, "    Format             = %08x", pContext->ctxInnerLoop.dwFormat);
        dbgDPFAndDump(flag, "    Filter             = %08x", pContext->ctxInnerLoop.dwFilter);
        dbgDPFAndDump(flag, "    Blend              = %08x", pContext->ctxInnerLoop.dwBlend);
        dbgDPFAndDump(flag, "    Control            = %08x", pContext->ctxInnerLoop.dwControl);
        dbgDPFAndDump(flag, "    FogColor           = %08x", pContext->ctxInnerLoop.dwFogColor);
        dbgDPFAndDump(flag, "");
    }

    bDPF = dbgShowState & NVDBG_SHOW_DX6_CLASS_STATE;
    bDump = dbgDumpState & NVDBG_SHOW_DX6_CLASS_STATE;
    flag = NVDBG_SHOW_DX6_CLASS_STATE;

    if ((bDPF || bDump) &&
        (dwFlags & NVDBG_SHOW_DX6_CLASS_STATE)) {
        dbgDPFAndDump(flag, "D3D:NV4 DX6 Class Context State:");
        dbgDPFAndDump(flag, "    TextureOffset[0]   = %08x", pContext->mtsState.dwTextureOffset[0]);
        dbgDPFAndDump(flag, "    TextureOffset[1]   = %08x", pContext->mtsState.dwTextureOffset[1]);
        dbgDPFAndDump(flag, "    TextureFormat[0]   = %08x", pContext->mtsState.dwTextureFormat[0]);
        dbgDPFAndDump(flag, "    TextureFormat[1]   = %08x", pContext->mtsState.dwTextureFormat[1]);
        dbgDPFAndDump(flag, "    TextureFilter[0]   = %08x", pContext->mtsState.dwTextureFilter[0]);
        dbgDPFAndDump(flag, "    TextureFilter[1]   = %08x", pContext->mtsState.dwTextureFilter[1]);
        dbgDPFAndDump(flag, "    Combine0Alpha      = %08x", pContext->mtsState.dwCombine0Alpha);
        dbgDPFAndDump(flag, "    Combine0Color      = %08x", pContext->mtsState.dwCombine0Color);
        dbgDPFAndDump(flag, "    Combine1Alpha      = %08x", pContext->mtsState.dwCombine1Alpha);
        dbgDPFAndDump(flag, "    Combine1Color      = %08x", pContext->mtsState.dwCombine1Color);
        dbgDPFAndDump(flag, "    CombineFactor      = %08x", pContext->mtsState.dwCombineFactor);
        dbgDPFAndDump(flag, "    Blend              = %08x", pContext->mtsState.dwBlend);
        dbgDPFAndDump(flag, "    Control0           = %08x", pContext->mtsState.dwControl0);
        dbgDPFAndDump(flag, "    Control1           = %08x", pContext->mtsState.dwControl1);
        dbgDPFAndDump(flag, "    Control2           = %08x", pContext->mtsState.dwControl2);
        dbgDPFAndDump(flag, "    FogColor           = %08x", pContext->mtsState.dwFogColor);
        dbgDPFAndDump(flag, "");
    }

    bDPF = dbgShowState & NVDBG_SHOW_SURFACE_STATE;
    bDump = dbgDumpState & NVDBG_SHOW_SURFACE_STATE;
    flag = NVDBG_SHOW_SURFACE_STATE;

    if ((bDPF || bDump) &&
        (dwFlags & NVDBG_SHOW_SURFACE_STATE)) {
        dbgDPFAndDump(flag, "D3D:NV4 Surface State:");
        dbgDPFAndDump(flag, "    ClipStart (x, y)   = %04d, %04d", pContext->surfaceViewport.clipHorizontal.wX, pContext->surfaceViewport.clipVertical.wY);
        dbgDPFAndDump(flag, "    ClipSize (w, h)    = %04d, %04d", pContext->surfaceViewport.clipHorizontal.wWidth, pContext->surfaceViewport.clipVertical.wHeight);
        dbgDPFAndDump(flag, "    SurfaceAddr        = %08x",       pContext->pRenderTarget->getAddress());
        dbgDPFAndDump(flag, "    SurfaceOffset      = %08x",       pContext->pRenderTarget->getOffset());
        dbgDPFAndDump(flag, "    SurfacePitch       = %04x",       pContext->pRenderTarget->getPitch());
        dbgDPFAndDump(flag, "    SurfaceFormat      = %08x",       pContext->pRenderTarget->getFormat());
        dbgDPFAndDump(flag, "    SurfaceBPP         = %08x",       pContext->pRenderTarget->getBPP());
        dbgDPFAndDump(flag, "    Z-BufferAddr       = %08x",       pContext->pZetaBuffer->getAddress());
        dbgDPFAndDump(flag, "    Z-BufferOffset     = %08x",       pContext->pZetaBuffer->getOffset());
        dbgDPFAndDump(flag, "    Z-BufferPitch      = %04x",       pContext->pZetaBuffer->getPitch());
        dbgDPFAndDump(flag, "");
    }

    return;
}

//-------------------------------------------------------------------------

void dbgShowRenderStateChange (BOOL bChanged, DWORD dwStateType, DWORD dwStateValue)
{
    if (dbgShowState & NVDBG_SHOW_RENDER_STATE) {
        if (bChanged) {
            DPF("nvSetContextState: Render State changed");
        }
        else {
            DPF("nvSetContextState: Render State unchanged");
        }
        DPF("  State = %s (%08d)", szD3DRenderStateLookupTable[dwStateType], dwStateType);
        DPF("  Data  = 0x%08x",    dwStateValue);
    }
}

//-------------------------------------------------------------------------

void dbgDisplayDrawPrimitives2Info (BYTE bCommand, WORD wCommandCount)
{
    BOOL  bDPF, bDump;
    DWORD flag;

    bDPF  = dbgShowState & NVDBG_SHOW_DRAWPRIM2_INFO;
    bDump = dbgDumpState & NVDBG_SHOW_DRAWPRIM2_INFO;
    flag  = NVDBG_SHOW_DRAWPRIM2_INFO;

    if (!(bDPF || bDump)) return;

    if (bCommand <= D3D_DP2OP_MAX) {
        dbgDPFAndDump(flag, "%s (%d) : %08x", szD3DDP2OpLookupTable[bCommand], (DWORD)bCommand, (DWORD)wCommandCount);
    }
    else {
        dbgDPFAndDump(flag, "%s (%d) : %08x", "D3DDP2OP_UNKNOWN", (DWORD)bCommand, (DWORD)wCommandCount);
    }
}

//-------------------------------------------------------------------------

void dbgDisplayVertexData
(
    BOOL    dx6,      // dx6 ? two sets of texture coordinates : one set
    DWORD   *pVertex  // array of vertex components (e.g. D3DTLVERTEX)
)
{
    static DWORD dbgVertexCount = 0;
    BOOL   bDPF, bDump;
    DWORD  flag;

    static float xmin   =  100000.0;
    static float ymin   =  100000.0;
    static float zmin   =  100000.0;
    static float rhwmin =  100000.0;
    static float xmax   = -100000.0;
    static float ymax   = -100000.0;
    static float zmax   = -100000.0;
    static float rhwmax = -100000.0;

    bDPF  = dbgShowState & NVDBG_SHOW_VERTICES;
    bDump = dbgDumpState & NVDBG_SHOW_VERTICES;
    flag  = NVDBG_SHOW_VERTICES;

    dbgVertexCount++;

    if (bDPF || bDump) {
        dbgDPFAndDump(flag, "D3D:NV4 Vertex Data (#%d):", dbgVertexCount);
        dbgDPFAndDump(flag, "    X        = %f",    FLOAT_FROM_DWORD(pVertex[0]));
        dbgDPFAndDump(flag, "    Y        = %f",    FLOAT_FROM_DWORD(pVertex[1]));
        dbgDPFAndDump(flag, "    Z        = %f",    FLOAT_FROM_DWORD(pVertex[2]));
        dbgDPFAndDump(flag, "    RHW      = %f",    FLOAT_FROM_DWORD(pVertex[3]));
        dbgDPFAndDump(flag, "    Diffuse  = %08x",  pVertex[4]);
        dbgDPFAndDump(flag, "    Specular = %08x",  pVertex[5]);
        dbgDPFAndDump(flag, "    U        = %f",    FLOAT_FROM_DWORD(pVertex[6]));
        dbgDPFAndDump(flag, "    V        = %f",    FLOAT_FROM_DWORD(pVertex[7]));

        if (dx6) {
            dbgDPFAndDump(flag, "    U2       = %f", FLOAT_FROM_DWORD(pVertex[8]));
            dbgDPFAndDump(flag, "    V2       = %f", FLOAT_FROM_DWORD(pVertex[9]));
        }

        dbgDPFAndDump(flag, "");
    }

    bDPF  = dbgShowState & NVDBG_SHOW_VERTEX_MAX_MIN;;
    bDump = dbgDumpState & NVDBG_SHOW_VERTEX_MAX_MIN;;
    flag  = NVDBG_SHOW_VERTEX_MAX_MIN;

    if (bDPF || bDump) {

        float f;
        f = FLOAT_FROM_DWORD(pVertex[0]);
        if (f > xmax) xmax = f;
        if (f < xmin) xmin = f;
        f = FLOAT_FROM_DWORD(pVertex[1]);
        if (f > ymax) ymax = f;
        if (f < ymin) ymin = f;
        f = FLOAT_FROM_DWORD(pVertex[2]);
        if (f > zmax) zmax = f;
        if (f < zmin) zmin = f;
        f = FLOAT_FROM_DWORD(pVertex[3]);
        if (f > rhwmax) rhwmax = f;
        if (f < rhwmin) rhwmin = f;

        dbgDPFAndDump(flag,"Minimum / Maximum Values");
        dbgDPFAndDump(flag,"  X   => %f / %f", xmin,xmax);
        dbgDPFAndDump(flag,"  Y   => %f / %f", ymin,ymax);
        dbgDPFAndDump(flag,"  Z   => %f / %f", zmin,zmax);
        dbgDPFAndDump(flag,"  RHW => %f / %f", rhwmin,rhwmax);
        dbgDPFAndDump(flag,"\r\n");

    } else {

        // if NVDBG_SHOW_VERTEX_MAX_MIN is off, then reset values
        xmin   =  100000.0;
        ymin   =  100000.0;
        zmin   =  100000.0;
        rhwmin =  100000.0;
        xmax   = -100000.0;
        ymax   = -100000.0;
        zmax   = -100000.0;
        rhwmax = -100000.0;

    }

}

//-------------------------------------------------------------------------

void dbgDisplayLight
(
    LPD3DLIGHT7 pLight7
)
{
    if (dbgShowState & NVDBG_SHOW_LIGHTS)
    {
        static DWORD dwCount = 0;
        dwCount++;

        //if ((dwCount == 1) || (dwCount == 30))
        {
            DPF ("D3DLIGHT7 (call #%d):-  (note: large floats will not be printed correctly)",dwCount);
            switch (pLight7->dltType)
            {
                case D3DLIGHT_SPOT:         DPF ("    type: SPOT");
                                            break;
                case D3DLIGHT_POINT:        DPF ("    type: POINT");
                                            break;
                case D3DLIGHT_DIRECTIONAL:  DPF ("    type: DIRECTIONAL");
                                            break;
            }
            DPF (" diffuse: ARGB = %02x %02x %02x %02x",
                      int(255.0 * pLight7->dcvDiffuse.a),
                      int(255.0 * pLight7->dcvDiffuse.r),
                      int(255.0 * pLight7->dcvDiffuse.g),
                      int(255.0 * pLight7->dcvDiffuse.b));
            DPF ("specular: ARGB = %02x %02x %02x %02x",
                      int(255.0 * pLight7->dcvSpecular.a),
                      int(255.0 * pLight7->dcvSpecular.r),
                      int(255.0 * pLight7->dcvSpecular.g),
                      int(255.0 * pLight7->dcvSpecular.b));
            DPF (" ambient: ARGB = %02x %02x %02x %02x",
                      int(255.0 * pLight7->dcvAmbient.a),
                      int(255.0 * pLight7->dcvAmbient.r),
                      int(255.0 * pLight7->dcvAmbient.g),
                      int(255.0 * pLight7->dcvAmbient.b));

            DPF ("position: %d.%04d %d.%04d %d.%04d",
                      int(pLight7->dvPosition.x),ABS(int(pLight7->dvPosition.x * 10000) % 10000),
                      int(pLight7->dvPosition.y),ABS(int(pLight7->dvPosition.y * 10000) % 10000),
                      int(pLight7->dvPosition.z),ABS(int(pLight7->dvPosition.z * 10000) % 10000));

            DPF ("     dir: %d.%04d %d.%04d %d.%04d",
                      int(pLight7->dvDirection.x),ABS(int(pLight7->dvDirection.x * 10000) % 10000),
                      int(pLight7->dvDirection.y),ABS(int(pLight7->dvDirection.y * 10000) % 10000),
                      int(pLight7->dvDirection.z),ABS(int(pLight7->dvDirection.z * 10000) % 10000));

            DPF ("   range: %d.%04d",int(pLight7->dvRange),ABS(int(pLight7->dvRange * 10000) % 10000));
            DPF (" falloff: %d.%04d",int(pLight7->dvFalloff),ABS(int(pLight7->dvFalloff * 10000) % 10000));
            DPF ("    att0: %d.%04d",int(pLight7->dvAttenuation0),ABS(int(pLight7->dvAttenuation0 * 10000) % 10000));
            DPF ("    att1: %d.%04d",int(pLight7->dvAttenuation1),ABS(int(pLight7->dvAttenuation1 * 10000) % 10000));
            DPF ("    att2: %d.%04d",int(pLight7->dvAttenuation2),ABS(int(pLight7->dvAttenuation2 * 10000) % 10000));
            DPF ("   theta: %d.%04d",int(pLight7->dvTheta),ABS(int(pLight7->dvTheta * 10000) % 10000));
            DPF ("     phi: %d.%04d",int(pLight7->dvPhi),ABS(int(pLight7->dvPhi * 10000) % 10000));
        }
    }
}

//-------------------------------------------------------------------------
// STATE VALIDATION
//-------------------------------------------------------------------------

BOOL dbgValidateControlRegisters
(
    PNVD3DCONTEXT   pContext
)
{
    DWORD   drfValue;

    nvAssert (!(pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS) &&
              !(pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN));

    /*
     * Validate the Format register.
     */
    drfValue = DRF_VAL(054, _FORMAT, _CONTEXT_DMA, pContext->ctxInnerLoop.dwFormat);
    if ((drfValue != NV054_FORMAT_CONTEXT_DMA_A)
        && (drfValue != NV054_FORMAT_CONTEXT_DMA_B)) {
        DPF("dbgValidateControlRegisters: NV054_FORMAT_CONTEXT_DMA field invalid 0x%08x", drfValue);
        dbgD3DError();
    }
    drfValue = DRF_VAL(054, _FORMAT, _COLORKEYENABLE, pContext->ctxInnerLoop.dwFormat);
    if ((drfValue != NV054_FORMAT_COLORKEYENABLE_FALSE)
        && (drfValue != NV054_FORMAT_COLORKEYENABLE_TRUE)) {
        DPF("dbgValidateControlRegisters: NV054_FORMAT_COLORKEYENABLE field invalid 0x%08x", drfValue);
        dbgD3DError();
    }
    drfValue = DRF_VAL(054, _FORMAT, _ORIGIN_ZOH, pContext->ctxInnerLoop.dwFormat);
    if ((drfValue != NV054_FORMAT_ORIGIN_ZOH_CENTER)
        && (drfValue != NV054_FORMAT_ORIGIN_ZOH_CORNER)) {
        DPF("dbgValidateControlRegisters: NV054_FORMAT__ORIGIN_ZOH field invalid 0x%08x", drfValue);
        dbgD3DError();
    }
    drfValue = DRF_VAL(054, _FORMAT, _ORIGIN_FOH, pContext->ctxInnerLoop.dwFormat);
    if ((drfValue != NV054_FORMAT_ORIGIN_FOH_CENTER)
        && (drfValue != NV054_FORMAT_ORIGIN_FOH_CORNER)) {
        DPF("dbgValidateControlRegisters: NV054_FORMAT__ORIGIN_FOH field invalid 0x%08x", drfValue);
        dbgD3DError();
    }
    drfValue = DRF_VAL(054, _FORMAT, _COLOR, pContext->ctxInnerLoop.dwFormat);
    if ((drfValue != NV054_FORMAT_COLOR_LE_Y8)
        && (drfValue != NV054_FORMAT_COLOR_LE_A1R5G5B5)
        && (drfValue != NV054_FORMAT_COLOR_LE_X1R5G5B5)
        && (drfValue != NV054_FORMAT_COLOR_LE_A4R4G4B4)
        && (drfValue != NV054_FORMAT_COLOR_LE_R5G6B5)
        && (drfValue != NV054_FORMAT_COLOR_LE_A8R8G8B8)
        && (drfValue != NV054_FORMAT_COLOR_LE_X8R8G8B8)) {
        DPF("dbgValidateControlRegisters: NV054_FORMAT_COLOR field invalid 0x%08x", drfValue);
        dbgD3DError();
    }
//    drfValue = DRF_VAL(054, _FORMAT, _MIPMAP_LEVELS, pContext->ctxInnerLoop.dwFormat);
//    if (drfValue )
//    {
//        DPF("dbgValidateControlRegisters: NV054_FORMAT_MIPMAP_LEVELS field invalid 0x%08x", drfValue);
//        dbgD3DError();
//    }
    drfValue = DRF_VAL(054, _FORMAT, _BASE_SIZE_U, pContext->ctxInnerLoop.dwFormat);
    if ((drfValue != NV054_FORMAT_BASE_SIZE_U_1)
        && (drfValue != NV054_FORMAT_BASE_SIZE_U_2)
        && (drfValue != NV054_FORMAT_BASE_SIZE_U_4)
        && (drfValue != NV054_FORMAT_BASE_SIZE_U_8)
        && (drfValue != NV054_FORMAT_BASE_SIZE_U_16)
        && (drfValue != NV054_FORMAT_BASE_SIZE_U_32)
        && (drfValue != NV054_FORMAT_BASE_SIZE_U_64)
        && (drfValue != NV054_FORMAT_BASE_SIZE_U_128)
        && (drfValue != NV054_FORMAT_BASE_SIZE_U_256)
        && (drfValue != NV054_FORMAT_BASE_SIZE_U_512)
        && (drfValue != NV054_FORMAT_BASE_SIZE_U_1024)
        && (drfValue != NV054_FORMAT_BASE_SIZE_U_2048)) {
        DPF("dbgValidateControlRegisters: NV054_FORMAT_BASE_SIZE_U field invalid 0x%08x", drfValue);
        dbgD3DError();
    }
    drfValue = DRF_VAL(054, _FORMAT, _BASE_SIZE_V, pContext->ctxInnerLoop.dwFormat);
    if ((drfValue != NV054_FORMAT_BASE_SIZE_V_1)
        && (drfValue != NV054_FORMAT_BASE_SIZE_V_2)
        && (drfValue != NV054_FORMAT_BASE_SIZE_V_4)
        && (drfValue != NV054_FORMAT_BASE_SIZE_V_8)
        && (drfValue != NV054_FORMAT_BASE_SIZE_V_16)
        && (drfValue != NV054_FORMAT_BASE_SIZE_V_32)
        && (drfValue != NV054_FORMAT_BASE_SIZE_V_64)
        && (drfValue != NV054_FORMAT_BASE_SIZE_V_128)
        && (drfValue != NV054_FORMAT_BASE_SIZE_V_256)
        && (drfValue != NV054_FORMAT_BASE_SIZE_V_512)
        && (drfValue != NV054_FORMAT_BASE_SIZE_V_1024)
        && (drfValue != NV054_FORMAT_BASE_SIZE_V_2048)) {
        DPF("dbgValidateControlRegisters: NV054_FORMAT_BASE_SIZE_V field invalid 0x%08x", drfValue);
        dbgD3DError();
    }
    drfValue = DRF_VAL(054, _FORMAT, _TEXTUREADDRESSU, pContext->ctxInnerLoop.dwFormat);
    if ((drfValue != NV054_FORMAT_TEXTUREADDRESSU_WRAP)
        && (drfValue != NV054_FORMAT_TEXTUREADDRESSU_MIRROR)
        && (drfValue != NV054_FORMAT_TEXTUREADDRESSU_CLAMP)
        && (drfValue != NV054_FORMAT_TEXTUREADDRESSU_BORDER)) {
        DPF("dbgValidateControlRegisters: NV054_FORMAT_TEXTUREADDRESSU field invalid 0x%08x", drfValue);
        dbgD3DError();
    }
    drfValue = DRF_VAL(054, _FORMAT, _WRAPU, pContext->ctxInnerLoop.dwFormat);
    if ((drfValue != NV054_FORMAT_WRAPU_FALSE)
        && (drfValue != NV054_FORMAT_WRAPU_TRUE)) {
        DPF("dbgValidateControlRegisters: NV054_FORMAT_WRAPU field invalid 0x%08x", drfValue);
        dbgD3DError();
    }
    drfValue = DRF_VAL(054, _FORMAT, _TEXTUREADDRESSV, pContext->ctxInnerLoop.dwFormat);
    if ((drfValue != NV054_FORMAT_TEXTUREADDRESSV_WRAP)
        && (drfValue != NV054_FORMAT_TEXTUREADDRESSV_MIRROR)
        && (drfValue != NV054_FORMAT_TEXTUREADDRESSV_CLAMP)
        && (drfValue != NV054_FORMAT_TEXTUREADDRESSV_BORDER)) {
        DPF("dbgValidateControlRegisters: NV054_FORMAT_TEXTUREADDRESSV field invalid 0x%08x", drfValue);
        dbgD3DError();
    }
    drfValue = DRF_VAL(054, _FORMAT, _WRAPV, pContext->ctxInnerLoop.dwFormat);
    if ((drfValue != NV054_FORMAT_WRAPV_FALSE)
        && (drfValue != NV054_FORMAT_WRAPV_TRUE)) {
        DPF("dbgValidateControlRegisters: NV054_FORMAT_WRAPV field invalid 0x%08x", drfValue);
        dbgD3DError();
    }

    // Validate the Filter register.
    drfValue = DRF_VAL(054, _FILTER, _MIPMAP_DITHER_ENABLE, pContext->ctxInnerLoop.dwFilter);
    if ((drfValue != NV054_FILTER_MIPMAP_DITHER_ENABLE_FALSE)
        && (drfValue != NV054_FILTER_MIPMAP_DITHER_ENABLE_TRUE)) {
        DPF("dbgValidateControlRegisters: NV054_FILTER_MIPMAP_DITHER_ENABLE field invalid 0x%08x", drfValue);
        dbgD3DError();
    }
    drfValue = DRF_VAL(054, _FILTER, _TEXTUREMIN, pContext->ctxInnerLoop.dwFilter);
    if ((drfValue != NV054_FILTER_TEXTUREMIN_NEAREST)
        && (drfValue != NV054_FILTER_TEXTUREMIN_LINEAR)
        && (drfValue != NV054_FILTER_TEXTUREMIN_MIPNEAREST)
        && (drfValue != NV054_FILTER_TEXTUREMIN_MIPLINEAR)
        && (drfValue != NV054_FILTER_TEXTUREMIN_LINEARMIPNEAREST)
        && (drfValue != NV054_FILTER_TEXTUREMIN_LINEARMIPLINEAR)) {
        DPF("dbgValidateControlRegisters: NV054_FILTER_TEXTUREMIN field invalid 0x%08x", drfValue);
        dbgD3DError();
    }
    drfValue = DRF_VAL(054, _FILTER, _ANISOTROPIC_MINIFY_ENABLE, pContext->ctxInnerLoop.dwFilter);
    if ((drfValue != NV054_FILTER_ANISOTROPIC_MINIFY_ENABLE_FALSE)
        && (drfValue != NV054_FILTER_ANISOTROPIC_MINIFY_ENABLE_TRUE)) {
        DPF("dbgValidateControlRegisters: NV054_FILTER_ANISOTROPIC_MINIFY_ENABLE field invalid 0x%08x", drfValue);
        dbgD3DError();
    }
    drfValue = DRF_VAL(054, _FILTER, _TEXTUREMAG, pContext->ctxInnerLoop.dwFilter);
    if ((drfValue != NV054_FILTER_TEXTUREMAG_NEAREST)
        && (drfValue != NV054_FILTER_TEXTUREMAG_LINEAR)
        && (drfValue != NV054_FILTER_TEXTUREMAG_MIPNEAREST)
        && (drfValue != NV054_FILTER_TEXTUREMAG_MIPLINEAR)
        && (drfValue != NV054_FILTER_TEXTUREMAG_LINEARMIPNEAREST)
        && (drfValue != NV054_FILTER_TEXTUREMAG_LINEARMIPLINEAR)) {
        DPF("dbgValidateControlRegisters: NV054_FILTER_TEXTUREMAG field invalid 0x%08x", drfValue);
        dbgD3DError();
    }
    drfValue = DRF_VAL(054, _FILTER, _ANISOTROPIC_MAGNIFY_ENABLE, pContext->ctxInnerLoop.dwFilter);
    if ((drfValue != NV054_FILTER_ANISOTROPIC_MAGNIFY_ENABLE_FALSE)
        && (drfValue != NV054_FILTER_ANISOTROPIC_MAGNIFY_ENABLE_TRUE)) {
        DPF("dbgValidateControlRegisters: NV054_FILTER_ANISOTROPIC_MAGNIFY_ENABLE field invalid 0x%08x", drfValue);
        dbgD3DError();
    }

    // Validate the Blend register.
    drfValue = DRF_VAL(054, _BLEND, _TEXTUREMAPBLEND, pContext->ctxInnerLoop.dwBlend);
    if ((drfValue != NV054_BLEND_TEXTUREMAPBLEND_DECAL)
        && (drfValue != NV054_BLEND_TEXTUREMAPBLEND_MODULATE)
        && (drfValue != NV054_BLEND_TEXTUREMAPBLEND_DECALALPHA)
        && (drfValue != NV054_BLEND_TEXTUREMAPBLEND_MODULATEALPHA)
        && (drfValue != NV054_BLEND_TEXTUREMAPBLEND_DECALMASK)
        && (drfValue != NV054_BLEND_TEXTUREMAPBLEND_MODULATEMASK)
        && (drfValue != NV054_BLEND_TEXTUREMAPBLEND_COPY)
        && (drfValue != NV054_BLEND_TEXTUREMAPBLEND_ADD)) {
        DPF("dbgValidateControlRegisters: NV054_BLEND_TEXTUREMAPBLEND field invalid 0x%08x", drfValue);
        dbgD3DError();
    }
    drfValue = DRF_VAL(054, _BLEND, _MASK_BIT, pContext->ctxInnerLoop.dwBlend);
    if ((drfValue != NV054_BLEND_MASK_BIT_LSB)
        && (drfValue != NV054_BLEND_MASK_BIT_MSB)) {
        DPF("dbgValidateControlRegisters: NV054_BLEND_MASK_BIT field invalid 0x%08x", drfValue);
        dbgD3DError();
    }
    drfValue = DRF_VAL(054, _BLEND, _SHADEMODE, pContext->ctxInnerLoop.dwBlend);
    if ((drfValue != NV054_BLEND_SHADEMODE_FLAT)
        && (drfValue != NV054_BLEND_SHADEMODE_GOURAUD)
        && (drfValue != NV054_BLEND_SHADEMODE_PHONG)) {
        DPF("dbgValidateControlRegisters: NV054_BLEND_SHADEMODE field invalid 0x%08x", drfValue);
        dbgD3DError();
    }
    drfValue = DRF_VAL(054, _BLEND, _TEXTUREPERSPECTIVE, pContext->ctxInnerLoop.dwBlend);
    if ((drfValue != NV054_BLEND_TEXTUREPERSPECTIVE_FALSE)
        && (drfValue != NV054_BLEND_TEXTUREPERSPECTIVE_TRUE)) {
        DPF("dbgValidateControlRegisters: NV054_BLEND_TEXTUREPERSPECTIVE field invalid 0x%08x", drfValue);
        dbgD3DError();
    }
    drfValue = DRF_VAL(054, _BLEND, _SPECULARENABLE, pContext->ctxInnerLoop.dwBlend);
    if ((drfValue != NV054_BLEND_SPECULARENABLE_FALSE)
        && (drfValue != NV054_BLEND_SPECULARENABLE_TRUE)) {
        DPF("dbgValidateControlRegisters: NV054_BLEND_SPECULARENABLE field invalid 0x%08x", drfValue);
        dbgD3DError();
    }
    drfValue = DRF_VAL(054, _BLEND, _FOGENABLE, pContext->ctxInnerLoop.dwBlend);
    if ((drfValue != NV054_BLEND_FOGENABLE_FALSE)
        && (drfValue != NV054_BLEND_FOGENABLE_TRUE)) {
        DPF("dbgValidateControlRegisters: NV054_BLEND_FOGENABLE field invalid 0x%08x", drfValue);
        dbgD3DError();
    }
    drfValue = DRF_VAL(054, _BLEND, _ALPHABLENDENABLE, pContext->ctxInnerLoop.dwBlend);
    if ((drfValue != NV054_BLEND_ALPHABLENDENABLE_FALSE)
        && (drfValue != NV054_BLEND_ALPHABLENDENABLE_TRUE)) {
        DPF("dbgValidateControlRegisters: NV054_BLEND_ALPHABLENDENABLE field invalid 0x%08x", drfValue);
        dbgD3DError();
    }
    drfValue = DRF_VAL(054, _BLEND, _SRCBLEND, pContext->ctxInnerLoop.dwBlend);
    if ((drfValue != NV054_BLEND_SRCBLEND_ZERO)
        && (drfValue != NV054_BLEND_SRCBLEND_ONE)
        && (drfValue != NV054_BLEND_SRCBLEND_SRCCOLOR)
        && (drfValue != NV054_BLEND_SRCBLEND_INVSRCCOLOR)
        && (drfValue != NV054_BLEND_SRCBLEND_SRCALPHA)
        && (drfValue != NV054_BLEND_SRCBLEND_INVSRCALPHA)
        && (drfValue != NV054_BLEND_SRCBLEND_DESTALPHA)
        && (drfValue != NV054_BLEND_SRCBLEND_INVDESTALPHA)
        && (drfValue != NV054_BLEND_SRCBLEND_DESTCOLOR)
        && (drfValue != NV054_BLEND_SRCBLEND_INVDESTCOLOR)
        && (drfValue != NV054_BLEND_SRCBLEND_SRCALPHASAT)) {
        DPF("dbgValidateControlRegisters: NV054_BLEND_SRCBLEND field invalid 0x%08x", drfValue);
        dbgD3DError();
    }
    drfValue = DRF_VAL(054, _BLEND, _DESTBLEND, pContext->ctxInnerLoop.dwBlend);
    if ((drfValue != NV054_BLEND_DESTBLEND_ZERO)
        && (drfValue != NV054_BLEND_DESTBLEND_ONE)
        && (drfValue != NV054_BLEND_DESTBLEND_SRCCOLOR)
        && (drfValue != NV054_BLEND_DESTBLEND_INVSRCCOLOR)
        && (drfValue != NV054_BLEND_DESTBLEND_SRCALPHA)
        && (drfValue != NV054_BLEND_DESTBLEND_INVSRCALPHA)
        && (drfValue != NV054_BLEND_DESTBLEND_DESTALPHA)
        && (drfValue != NV054_BLEND_DESTBLEND_INVDESTALPHA)
        && (drfValue != NV054_BLEND_DESTBLEND_DESTCOLOR)
        && (drfValue != NV054_BLEND_DESTBLEND_INVDESTCOLOR)
        && (drfValue != NV054_BLEND_DESTBLEND_SRCALPHASAT)) {
        DPF("dbgValidateControlRegisters: NV054_BLEND_DESTBLEND field invalid 0x%08x", drfValue);
        dbgD3DError();
    }

    // Validate the Control register.
    drfValue = DRF_VAL(054, _CONTROL, _ALPHAFUNC, pContext->ctxInnerLoop.dwControl);
    if ((drfValue != NV054_CONTROL_ALPHAFUNC_NEVER)
        && (drfValue != NV054_CONTROL_ALPHAFUNC_LESS)
        && (drfValue != NV054_CONTROL_ALPHAFUNC_EQUAL)
        && (drfValue != NV054_CONTROL_ALPHAFUNC_LESSEQUAL)
        && (drfValue != NV054_CONTROL_ALPHAFUNC_GREATER)
        && (drfValue != NV054_CONTROL_ALPHAFUNC_NOTEQUAL)
        && (drfValue != NV054_CONTROL_ALPHAFUNC_GREATEREQUAL)
        && (drfValue != NV054_CONTROL_ALPHAFUNC_ALWAYS)) {
        DPF("dbgValidateControlRegisters: NV054_CONTROL_ALPHAFUNC field invalid 0x%08x", drfValue);
        dbgD3DError();
    }
    drfValue = DRF_VAL(054, _CONTROL, _ALPHATESTENABLE, pContext->ctxInnerLoop.dwControl);
    if ((drfValue != NV054_CONTROL_ALPHATESTENABLE_FALSE)
        && (drfValue != NV054_CONTROL_ALPHATESTENABLE_TRUE)) {
        DPF("dbgValidateControlRegisters: NV054_CONTROL_ALPHATESTENABLE field invalid 0x%08x", drfValue);
        dbgD3DError();
    }
    drfValue = DRF_VAL(054, _CONTROL, _ZENABLE, pContext->ctxInnerLoop.dwControl);
    if ((drfValue != NV054_CONTROL_ZENABLE_FALSE)
        && (drfValue != NV054_CONTROL_ZENABLE_TRUE)) {
        DPF("dbgValidateControlRegisters: NV054_CONTROL_ZENABLE field invalid 0x%08x", drfValue);
        dbgD3DError();
    }
    drfValue = DRF_VAL(054, _CONTROL, _ZFUNC, pContext->ctxInnerLoop.dwControl);
    if ((drfValue != NV054_CONTROL_ZFUNC_NEVER)
        && (drfValue != NV054_CONTROL_ZFUNC_LESS)
        && (drfValue != NV054_CONTROL_ZFUNC_EQUAL)
        && (drfValue != NV054_CONTROL_ZFUNC_LESSEQUAL)
        && (drfValue != NV054_CONTROL_ZFUNC_GREATER)
        && (drfValue != NV054_CONTROL_ZFUNC_NOTEQUAL)
        && (drfValue != NV054_CONTROL_ZFUNC_GREATEREQUAL)
        && (drfValue != NV054_CONTROL_ZFUNC_ALWAYS)) {
        DPF("dbgValidateControlRegisters: NV054_CONTROL_ZFUNC field invalid 0x%08x", drfValue);
        dbgD3DError();
    }
    drfValue = DRF_VAL(054, _CONTROL, _CULLMODE, pContext->ctxInnerLoop.dwControl);
    if ((drfValue != NV054_CONTROL_CULLMODE_NONE)
        && (drfValue != NV054_CONTROL_CULLMODE_CW)
        && (drfValue != NV054_CONTROL_CULLMODE_CCW)) {
        DPF("dbgValidateControlRegisters: NV054_CONTROL_CULLMODE field invalid 0x%08x", drfValue);
        dbgD3DError();
    }
    drfValue = DRF_VAL(054, _CONTROL, _DITHERENABLE, pContext->ctxInnerLoop.dwControl);
    if ((drfValue != NV054_CONTROL_DITHERENABLE_FALSE)
        && (drfValue != NV054_CONTROL_DITHERENABLE_TRUE)) {
        DPF("dbgValidateControlRegisters: NV054_CONTROL_DITHERENABLE field invalid 0x%08x", drfValue);
        dbgD3DError();
    }
    drfValue = DRF_VAL(054, _CONTROL, _Z_PERSPECTIVE_ENABLE, pContext->ctxInnerLoop.dwControl);
    if ((drfValue != NV054_CONTROL_Z_PERSPECTIVE_ENABLE_FALSE)
        && (drfValue != NV054_CONTROL_Z_PERSPECTIVE_ENABLE_TRUE)) {
        DPF("dbgValidateControlRegisters: NV054_CONTROL_Z_PERSPECTIVE_ENABLE field invalid 0x%08x", drfValue);
        dbgD3DError();
    }
    drfValue = DRF_VAL(054, _CONTROL, _ZWRITEENABLE, pContext->ctxInnerLoop.dwControl);
    if ((drfValue != NV054_CONTROL_ZWRITEENABLE_FALSE)
        && (drfValue != NV054_CONTROL_ZWRITEENABLE_TRUE)) {
        DPF("dbgValidateControlRegisters: NV054_CONTROL_ZWRITEENABLE field invalid 0x%08x", drfValue);
        dbgD3DError();
    }
    drfValue = DRF_VAL(054, _CONTROL, _Z_FORMAT, pContext->ctxInnerLoop.dwControl);
    if ((drfValue != NV054_CONTROL_Z_FORMAT_FIXED)
        && (drfValue != NV054_CONTROL_Z_FORMAT_FLOAT)) {
        DPF("dbgValidateControlRegisters: NV054_CONTROL_Z_FORMAT field invalid 0x%08x", drfValue);
        dbgD3DError();
    }
    return(TRUE);
}

#endif // DEBUG

//-------------------------------------------------------------------------
// TEXTURE PREVIEWING
//-------------------------------------------------------------------------

#ifdef DEBUG

// display a particular texture mipmap level at screen position (dwX, dwY)

#define CLEAR_BORDER_SIZE 24

// set one pixel's worth of data from pSrc, subject to dwBPP
#define SETPIXEL1(pPix, pSrc, dwBPP)                \
{                                                   \
    if ((dwBPP) == 2) {                             \
        *(WORD*)(pPix) = *(WORD*)(pSrc);            \
    }                                               \
    else {                                          \
        *(DWORD*)(pPix) = *(DWORD*)(pSrc);          \
    }                                               \
}

// set one pixel's data from BYTES a, r, g, and b, subject to dwBPP
#define SETPIXEL2(pPix, a,r,g,b, dwBPP)             \
{                                                   \
    if ((dwBPP) == 2) {                             \
        *(WORD*)(pPix) = ((((r) & 0xf8) << 8) |     \
                          (((g) & 0xfc) << 3) |     \
                          (((b) & 0xf8) >> 3));     \
    }                                               \
    else {                                          \
        *(DWORD*)(pPix) = (((a) << 24) |            \
                           ((r) << 16) |            \
                           ((g) <<  8) |            \
                           ((b) <<  0));            \
    }                                               \
}

//---------------------------------------------------------------------------

void dbgTextureDisplayLevel
(
    CTexture *pTexture,
    DWORD     dwX,           // horizontal location for output
    DWORD     dwY,           // vertical location for output
    DWORD     dwClearColor
)
{
    DWORD u, v, u0, v0, uu, vv;
    DWORD dwNonZeroAlphaCount = 0;
    DWORD dwTexWidth, dwTexHeight, dwTexPitch;
    DWORD dwTexLogicalWidth, dwTexLogicalHeight;
    DWORD dwXOffset, dwXOffsetMax, dwYMax;
    DWORD dwFBOffset, dwFBIndexShift;
    DWORD dwTexelIndex, dwTexIndexShift;
    DWORD dwColors, dwBits;
    PBYTE pTextureBase, pTexel, pPalette;
    PBYTE pFB, pFB0;
    float a0, r0, g0, b0;
    float a1, r1, g1, b1;
    float da, dr, dg, db;
    WORD  wInterpIndex, wInterp;
    BYTE  a, r, g, b;
    BOOL  bLinear;

    bLinear = (dbgTexturePreview & NVDBG_TEXTURE_PREVIEW_LINEAR) ? TRUE : FALSE;

    dwTexWidth         = pTexture->getWidth();
    dwTexLogicalWidth  = pTexture->getLogicalWidth();
    dwTexHeight        = pTexture->getHeight();
    dwTexLogicalHeight = pTexture->getLogicalHeight();
    dwTexPitch         = pTexture->getPitch();

    // make sure we don't draw outside the primary
    dwXOffset    = dwX * dbgPrimaryBPP;
    dwXOffsetMax = dbgPrimaryPitch  - dbgPrimaryBPP * (dwTexLogicalWidth + CLEAR_BORDER_SIZE);
    dwYMax       = dbgPrimaryHeight - (dwTexLogicalHeight + CLEAR_BORDER_SIZE);

    dwXOffset = min (dwXOffset, dwXOffsetMax);
    dwY       = min (dwY, dwYMax);

    dwFBOffset = dwY*dbgPrimaryPitch + dwXOffset;

    pFB  = (PBYTE) VIDMEM_ADDR(pDriverData->CurrentVisibleSurfaceAddress);
    pFB  = pFB + dwFBOffset;
    pFB0 = pFB;

    pTextureBase = bLinear ?
                   (PBYTE) (pTexture->getLinear()->getAddress()) :
                   (PBYTE) (pTexture->getSwizzled()->getAddress());

    if (!pTextureBase) {
        DPF ("dbgTextureDisplayLevel: couldn't get texture base. aborting preview");
        return;
    }

    dwTexIndexShift = pTexture->getBPP();
    ASM_LOG2 (dwTexIndexShift);

    dwFBIndexShift = dbgPrimaryBPP;
    ASM_LOG2 (dwFBIndexShift);

    // clear the area first

    for (v = 0; v < dwTexLogicalHeight+CLEAR_BORDER_SIZE; v++) {
        for (u = 0; u < dwTexLogicalWidth+CLEAR_BORDER_SIZE; u++) {
            SETPIXEL1 (pFB+(u<<dwFBIndexShift), &dwClearColor, dbgPrimaryBPP);
        }
        pFB += dbgPrimaryPitch;
    }
    pFB = pFB0;

    // render the texture

    if (pTexture->isDXT()) {

        // iterate over the blocks
        for (v=0; v < dwTexHeight; v++) {

            // start pFB in the upper-left corner of this row of 4x4 blocks
            pFB = pFB0 + (dbgPrimaryPitch * (v<<2));

            for (u=0; u < dwTexWidth; u++) {

                dwTexelIndex = dwTexWidth * v + u;
                pTexel = pTextureBase + (dwTexelIndex << dwTexIndexShift);

                switch (pTexture->getFormat()) {

                    case NV_SURFACE_FORMAT_DXT1_A1R5G5B5:
                        // ignore the 1-bit alpha sub-case of DXT1
                        dwColors = ((DWORD*)pTexel)[0];
                        dwBits   = ((DWORD*)pTexel)[1];
                        break;

                    case NV_SURFACE_FORMAT_DXT23_A8R8G8B8:
                    case NV_SURFACE_FORMAT_DXT45_A8R8G8B8:
                        // ignore the alpha component completely for DXT2-5
                        dwColors = ((DWORD*)pTexel)[2];
                        dwBits   = ((DWORD*)pTexel)[3];
                        break;

                    default:
                        DPF ("unhandled texture format in dbgTextureDisplay");
                        dbgD3DError();
                        return;
                        break;

                }  // switch

                a0 = (float) (0);
                r0 = (float) ((dwColors >> 8) & 0xf8);
                g0 = (float) ((dwColors >> 3) & 0xfc);
                b0 = (float) ((dwColors << 3) & 0xf8);

                a1 = (float) (0);
                r1 = (float) ((dwColors >> 24) & 0xf8);
                g1 = (float) ((dwColors >> 19) & 0xfc);
                b1 = (float) ((dwColors >> 13) & 0xf8);

                da = (float) ((a1 - a0) / 3);
                dr = (float) ((r1 - r0) / 3);
                dg = (float) ((g1 - g0) / 3);
                db = (float) ((b1 - b0) / 3);

                for (vv=0; vv<4; vv++) {
                    for (uu=0; uu<4; uu++) {
                        wInterpIndex = (WORD)(dwBits & 0x3);
                        wInterp = (wInterpIndex == 0x0) ? 0 :
                                  (wInterpIndex == 0x1) ? 3 :
                                  (wInterpIndex == 0x2) ? 1 : 2;
                        dwBits >>= 2;
                        a = (BYTE) (a0 + wInterp*da);
                        r = (BYTE) (r0 + wInterp*dr);
                        g = (BYTE) (g0 + wInterp*dg);
                        b = (BYTE) (b0 + wInterp*db);
                        SETPIXEL2 ((pFB + vv*dbgPrimaryPitch) + (uu<<dwFBIndexShift), a,r,g,b, dbgPrimaryBPP);
                    }
                }

                // move over to the next 4x4 block
                pFB += (4 << dwFBIndexShift);

            }  // u

        }  // v

    }

    else {  // !DXT

        // check for palettized surfaces
        if (pTexture->isPalettized()) {
            nvAssert (pTexture->getWrapper());
            if (pTexture->getWrapper()->getPalette()) {
                nvAssert (pTexture->getWrapper()->getPalette()->getPaletteSurface());
                pPalette = (PBYTE)(pTexture->getWrapper()->getPalette()->getPaletteSurface()->getAddress());
            } else {
                DPF("preview texture does not yet have a palette (normal if previewing from blit)");
                return;
            }
        }
        else if (pTexture->hasAutoPalette()) {
            pPalette = (PBYTE)(pTexture->getAutoPaletteOffset() +
                       pTexture->getSwizzled()->getAddress() -
                       pTexture->getSwizzled()->getOffset());
        }

        for (v=0; v < dwTexHeight; v++) {

            V_INTERLEAVE (v0, v, dwTexWidth, dwTexHeight);

            for (u=0; u < dwTexWidth; u++) {

                U_INTERLEAVE (u0, u, dwTexWidth, dwTexHeight);

                if (bLinear) {
                    pTexel = pTextureBase + (v * dwTexPitch) + (u << dwTexIndexShift);
                }
                else {
                    dwTexelIndex = u0 | v0;
                    pTexel = pTextureBase + (dwTexelIndex << dwTexIndexShift);
                }

                switch (pTexture->getFormat()) {

                    case NV_SURFACE_FORMAT_A1R5G5B5:

                        a = (BYTE)((*(WORD*)pTexel >> 8) & 0x80);
                        r = (BYTE)((*(WORD*)pTexel >> 7) & 0xf8);
                        g = (BYTE)((*(WORD*)pTexel >> 2) & 0xf8);
                        b = (BYTE)((*(WORD*)pTexel << 3) & 0xf8);
                        break;

                    case NV_SURFACE_FORMAT_X1R5G5B5:

                        a = (BYTE)(0xFF);
                        r = (BYTE)((*(WORD*)pTexel >> 7) & 0xf8);
                        g = (BYTE)((*(WORD*)pTexel >> 2) & 0xf8);
                        b = (BYTE)((*(WORD*)pTexel << 3) & 0xf8);
                        break;

                    case NV_SURFACE_FORMAT_A4R4G4B4:

                        a = (BYTE)((*(WORD*)pTexel >> 8) & 0xf0);
                        r = (BYTE)((*(WORD*)pTexel >> 4) & 0xf0);
                        g = (BYTE)((*(WORD*)pTexel >> 0) & 0xf0);
                        b = (BYTE)((*(WORD*)pTexel << 4) & 0xf0);
                        break;

                    case NV_SURFACE_FORMAT_R5G6B5:

                        a = (BYTE)(0xFF);
                        r = (BYTE)((*(WORD*)pTexel >> 8) & 0xf8);
                        g = (BYTE)((*(WORD*)pTexel >> 3) & 0xfc);
                        b = (BYTE)((*(WORD*)pTexel << 3) & 0xf8);
                        break;

                    case NV_SURFACE_FORMAT_A8R8G8B8:

                        a = (BYTE)((*(DWORD*)pTexel >> 24) & 0xff);
                        r = (BYTE)((*(DWORD*)pTexel >> 16) & 0xff);
                        g = (BYTE)((*(DWORD*)pTexel >>  8) & 0xff);
                        b = (BYTE)((*(DWORD*)pTexel >>  0) & 0xff);
                        break;

                    case NV_SURFACE_FORMAT_X8R8G8B8:

                        a = (BYTE)(0xFF);
                        r = (BYTE)((*(DWORD*)pTexel >> 16) & 0xff);
                        g = (BYTE)((*(DWORD*)pTexel >>  8) & 0xff);
                        b = (BYTE)((*(DWORD*)pTexel >>  0) & 0xff);
                        break;

                    case NV_SURFACE_FORMAT_DV8DU8:

                        a = (BYTE)(0xFF);
                        r = (BYTE)(0);
                        g = (BYTE)((*(WORD*)pTexel >> 8) & 0xff);
                        b = (BYTE)((*(WORD*)pTexel >> 0) & 0xff);
                        break;

                    case NV_SURFACE_FORMAT_L6DV5DU5:

                        a = (BYTE)(0xFF);
                        r = (BYTE)((*(WORD*)pTexel >> 8) & 0xfc);
                        g = (BYTE)((*(WORD*)pTexel >> 2) & 0xf8);
                        b = (BYTE)((*(WORD*)pTexel << 3) & 0xf8);
                        break;

                    case NV_SURFACE_FORMAT_X8L8DV8DU8:

                        a = (BYTE)(0xFF);
                        r = (BYTE)((*(DWORD*)pTexel >> 16) & 0xff);
                        g = (BYTE)((*(DWORD*)pTexel >>  8) & 0xff);
                        b = (BYTE)((*(DWORD*)pTexel >>  0) & 0xff);
                        break;

                    case NV_SURFACE_FORMAT_I8_A8R8G8B8:

                        nvAssert(pPalette);
                        dwColors = ((DWORD *)pPalette)[*pTexel];
                        a = (BYTE)((dwColors >> 24) & 0xff);
                        r = (BYTE)((dwColors >> 16) & 0xff);
                        g = (BYTE)((dwColors >>  8) & 0xff);
                        b = (BYTE)((dwColors >>  0) & 0xff);
                        break;

                    default:

                        DPF ("unhandled texture format in dbgTextureDisplay");
                        dbgD3DError();
                        return;
                        break;

                }  // switch

                if (dbgTexturePreview & NVDBG_TEXTURE_PREVIEW_ALPHA) r=g=b=a;
                if (a) dwNonZeroAlphaCount++;

                SETPIXEL2 (pFB+(u<<dwFBIndexShift), a,r,g,b, dbgPrimaryBPP);

            }  // loop on u

            pFB += dbgPrimaryPitch;

        }  // loop on v

    }  // !DXT

    if (pTexture->hasAlpha()) {
        if (dwNonZeroAlphaCount) {
            DPF_LEVEL (NVDBG_LEVEL_INFO, "dbgTextureDisplay - Non Zero Alpha Count = %08x of %08x",
                       dwNonZeroAlphaCount, dwTexWidth * dwTexHeight);
        }
        else {
            DPF_LEVEL (NVDBG_LEVEL_INFO, "dbgTextureDisplay - alpha texture has no alpha!!!!");
        }
    }
}

//---------------------------------------------------------------------------

void dbgTextureDisplay
(
    CTexture *pTexture
)
{
    CNvObject *pNvObjA, *pNvObjB;
    DWORD      dwX, dwY;
    DWORD      dwClearColor;

    nvAssert (pTexture);

    if (dbgTexturePreview & NVDBG_TEXTURE_PREVIEW_CUBE) {

        dwClearColor = (DWORD)(0xffffffff * ((double)rand() / RAND_MAX));
        dwX = 0;
        pNvObjB = pTexture->getWrapper();
        while (pNvObjB) {
            if (dbgTexturePreview & NVDBG_TEXTURE_PREVIEW_MIP) {
                dwY = 0;
                pNvObjA = pNvObjB;
                while (pNvObjA) {
                    dbgTextureDisplayLevel (pNvObjA->getTexture(), dwX, dwY, dwClearColor);
                    dwY += pNvObjA->getTexture()->getLogicalHeight();
                    pNvObjA = pNvObjA->getAttachedA();
                }
            }
            else {
                dbgTextureDisplayLevel (pNvObjB->getTexture(), dwX, 0, dwClearColor);
            }
            dwX += pNvObjB->getTexture()->getLogicalWidth();
            pNvObjB = pNvObjB->getAttachedB();
            dwClearColor = (DWORD)(0xffffffff * ((double)rand() / RAND_MAX));
        }

    }

    else if (dbgTexturePreview & NVDBG_TEXTURE_PREVIEW_MIP) {

        dwClearColor = (DWORD)(0xffffffff * ((double)rand() / RAND_MAX));
        dwY = 0;
        pNvObjA = pTexture->getWrapper();
        while (pNvObjA) {
            dbgTextureDisplayLevel (pNvObjA->getTexture(), 0, dwY, dwClearColor);
            dwY += pNvObjA->getTexture()->getLogicalHeight();
            pNvObjA = pNvObjA->getAttachedA();
        }

    }

    else if (dbgTexturePreview & NVDBG_TEXTURE_PREVIEW_BASE) {

        dwClearColor = (DWORD)(0xffffffff * ((double)rand() / RAND_MAX));
        dbgTextureDisplayLevel (pTexture, 0, 0, dwClearColor);

    }
}

#endif  // DEBUG

//-------------------------------------------------------------------------
// FRONT RENDERING
//-------------------------------------------------------------------------

#ifdef DEBUG

void dbgSetFrontRenderTarget (PNVD3DCONTEXT pContext, CSimpleSurface *pSurfaceToMimic)
{
    if (dbgFrontRender) {
        if (!pSurfaceToMimic->getWrapper()->getTexture()) {  // don't try to co-opt texture rendering
            dbgFrontRenderSurface.disown();
            dbgFrontRenderSurface.own (VIDMEM_ADDR(pDriverData->CurrentVisibleSurfaceAddress),
                                       dbgPrimaryPitch,
                                       pSurfaceToMimic->getHeight(),
                                       CSimpleSurface::HEAP_VID, FALSE);
            dbgFrontRenderSurface.setFormat (pSurfaceToMimic->getFormat());
            dbgFrontRenderSurface.setWidth (pSurfaceToMimic->getWidth());
            dbgFrontRenderSurface.setBPP (pSurfaceToMimic->getBPP());
            dbgFrontRenderSurface.setBPPRequested (pSurfaceToMimic->getBPPRequested());
            dbgFrontRenderSurface.setWrapper (pSurfaceToMimic->getWrapper());
            pContext->pRenderTarget = &(dbgFrontRenderSurface);
        }
    }
}

#endif // DEBUG

#endif  // NVARCH >= 0x04

