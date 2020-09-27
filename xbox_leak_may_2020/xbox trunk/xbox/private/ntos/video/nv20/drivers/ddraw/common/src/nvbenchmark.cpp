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
//  Module: nvBenchmark.cpp
//   Utility class for benchmarking and profiling purposes
//
//  History:
//      Menelaos Levas   1/30/2001
//
//
//
//
/********************************************************************/



#include "nvprecomp.h"
#ifdef NV_PROFILE_DP2OPS

char opLookupTable[D3D_DP2OP_MAX+1][64] =
{
    " ",                                           //  0
    "D3DDP2OP_POINTS",                              //  1
    "D3DDP2OP_INDEXEDLINELIST",                     //  2
    "D3DDP2OP_INDEXEDTRIANGLELIST",                 //  3
    "D3DDP2OP_RESERVED0",                           //  4
    " ",                                           //  5
    " ",                                           //  6
    " ",                                           //  7
    "D3DDP2OP_RENDERSTATE",                         //  8
    " ",                                           //  9
    " ",                                           // 10
    "D3DDP2OP_END",                                 // 11
    " ",                                           // 12
    " ",                                           // 13
    " ",                                           // 14
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

char opTypes[D3D_DP2OP_MAX+1] = 
{
    0,
    1,
    1,
    1,
    0,
    0,
    0,
    0,
    2,
    0,
    0,
    0,
    0,
    0,
    0,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    2,
    1,
    1,
    0,
    0,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    0,
    1,
    2,
    2,
    2,
    1,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    1,
    1,
    2,
    2,
    2,
    2,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    2
};


DWORD CBenchmark::resetData() {
    for (DWORD i = 0; i <= D3D_DP2OP_MAX; i++) {
        dwCallsCountTable[i] = 0;
        dwPrimsCountTable[i] = 0;
    }

    dwDrawingCalls = 0;
    dwStateCalls = 0;
    dwDrawingPrimCount = 0; 
    dwStatePrimCount = 0;
    dwCurrentFrame = 0;
    dwHWStateChanges = 0;
    dwStateBlocks = 0;
    dwDrawBlocks = 0;
    dwLastType = 0;

    dwStartPut = 0;
    dwHWStateChangeSize = 0;
    dwActiveTextures = 0;    
    dwLastPrimCount = 0;

    for (i = 0; i < 5; i++) 
        dwTexPrims[i] = 0;
    for (i = 0; i < 13; i++) 
        dwHWChanged[i] = 0;

    for (i = 0; i < 8192; i++)
        dwStateBits[i] = 0;
    
    return 0;
}

DWORD CBenchmark::logData(DWORD dwCommand, DWORD dwPrimCount) {
    dwCallsCountTable[dwCommand]++;
    dwPrimsCountTable[dwCommand] += dwPrimCount;
    if (opTypes[dwCommand] == 1) {
        dwDrawingCalls++;
        if (dwPrimCount > 1) {
            dwDrawingPrimCount += dwPrimCount;
            dwLastPrimCount = dwPrimCount;
        }
        if (dwLastType == 2)
            dwDrawBlocks++;
    }
    else if (opTypes[dwCommand] == 2) {
        dwStateCalls++;
        dwStatePrimCount += dwPrimCount;
        if (dwLastType == 1)
            dwStateBlocks++;
    }

    dwLastType = opTypes[dwCommand];

    return 0;
}

DWORD CBenchmark::logDrawPrimitive(DWORD dwCount) {
//    primitives += wCount;
    dwDrawingPrimCount += dwCount;
    dwLastPrimCount = dwCount;

    return 0;
}

DWORD CBenchmark::beginHWStateChange(DWORD dwFlags, DWORD dwPut) {

    dwStartPut = dwPut;    
    
#if (NVARCH >= 0x020)
    if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) {
        dwFlags &= KELVIN_DIRTY_REALLY_FILTHY;
        dwStateBits[dwFlags >> 16]++;
        if (dwFlags & KELVIN_DIRTY_COMBINERS_COLOR) dwHWChanged[0]++; 
        if (dwFlags & KELVIN_DIRTY_COMBINERS_SPECFOG) dwHWChanged[1]++;
        if (dwFlags & KELVIN_DIRTY_CONTROL0) dwHWChanged[2]++;
        if (dwFlags & KELVIN_DIRTY_DEFAULTS) dwHWChanged[3]++;
        if (dwFlags & KELVIN_DIRTY_FVF) dwHWChanged[4]++;
        if (dwFlags & KELVIN_DIRTY_LIGHTS) dwHWChanged[5]++;
        if (dwFlags & KELVIN_DIRTY_MISC_STATE) dwHWChanged[6]++;
        if (dwFlags & KELVIN_DIRTY_SURFACE) dwHWChanged[7]++;
        if (dwFlags & KELVIN_DIRTY_TEXTURE_STATE) { dwHWChanged[8]++; dwActiveTextures = 0; }
        if (dwFlags & KELVIN_DIRTY_TEXTURE_TRANSFORM) dwHWChanged[9]++;
        if (dwFlags & KELVIN_DIRTY_TRANSFORM) dwHWChanged[10]++;
        if (dwFlags & KELVIN_DIRTY_VERTEX_SHADER) dwHWChanged[11]++;
        if (dwFlags & KELVIN_DIRTY_PIXEL_SHADER) dwHWChanged[12]++;        
    } else
#endif
#if (NVARCH >= 0x010)
    {
        dwFlags &= CELSIUS_DIRTY_REALLY_FILTHY;
        if (dwFlags & CELSIUS_DIRTY_COMBINERS) dwHWChanged[0]++; 
        if (dwFlags & CELSIUS_DIRTY_SPECFOG_COMBINER) dwHWChanged[1]++;
        if (dwFlags & CELSIUS_DIRTY_CONTROL0) dwHWChanged[2]++;
        if (dwFlags & CELSIUS_DIRTY_DEFAULTS) dwHWChanged[3]++;
        if (dwFlags & CELSIUS_DIRTY_FVF) dwHWChanged[4]++;
        if (dwFlags & CELSIUS_DIRTY_LIGHTS) dwHWChanged[5]++;
        if (dwFlags & CELSIUS_DIRTY_MISC_STATE) dwHWChanged[6]++;
        if (dwFlags & CELSIUS_DIRTY_SURFACE) dwHWChanged[7]++;
        if (dwFlags & CELSIUS_DIRTY_TEXTURE_STATE) dwHWChanged[8]++;
        if (dwFlags & CELSIUS_DIRTY_TEXTURE_TRANSFORM) dwHWChanged[9]++;
        if (dwFlags & CELSIUS_DIRTY_TRANSFORM) dwHWChanged[10]++;
        if (dwFlags & CELSIUS_DIRTY_TL_MODE) dwHWChanged[11]++;
        if (dwFlags & CELSIUS_DIRTY_PIXEL_SHADER) dwHWChanged[12]++;    
    }
#endif

    dwHWStateChanges++;
    return 0;
}

DWORD CBenchmark::endHWStateChange(DWORD dwPut) {
    if (dwPut > dwStartPut)
        dwHWStateChangeSize += dwPut - dwStartPut;
    else 
        dwHWStateChangeSize += pDriverData->nvPusher.getBase() + pDriverData->nvPusher.getSize() - dwStartPut +
                                dwPut - pDriverData->nvPusher.getBase();

    dwTexPrims[dwActiveTextures] += dwLastPrimCount;

    return 0;
}

DWORD CBenchmark::incActiveTextures() {
    dwActiveTextures++;
    return 0;
} 

DWORD CBenchmark::outputData(DWORD dwFlags) {
    if ((dwFlags & OUTPUT_FRAMECOUNT) > dwCurrentFrame) {
        dwCurrentFrame++;
        return 0;
    }

    if (dwFlags & OUTPUT_ALL_DP2OPS) {
        for (DWORD i = 0; i <= D3D_DP2OP_MAX; i++) { 
            if ((opTypes[i] != 0) && (dwCallsCountTable[i] != 0)) 
                PF("%s: calls: %d, prims: %d, average: %f", opLookupTable[i], 
                    dwCallsCountTable[i], dwPrimsCountTable[i], 
                    dwPrimsCountTable[i]/((float)dwCallsCountTable[i]));
        }
        PF("*************************************************************");
    }

    if (dwFlags & OUTPUT_TOTALS) {
        PF("TOTALS:");
        PF("Total drawing calls: %d, prims: %d, average: %f",
            dwDrawingCalls, dwDrawingPrimCount, dwDrawingPrimCount/((float)dwDrawingCalls));

        if (dwTexPrims[0] > 0)
            PF("Untextured prims: %d (%f)", dwTexPrims[0], 100*dwTexPrims[0]/(float)dwDrawingPrimCount);
        if (dwTexPrims[1] > 0)
            PF("Single textured prims: %d (%f)", dwTexPrims[1], 100*dwTexPrims[1]/(float)dwDrawingPrimCount);
        if (dwTexPrims[2] > 0)
            PF("Dual textured prims: %d (%f)", dwTexPrims[2], 100*dwTexPrims[2]/(float)dwDrawingPrimCount);
        if (dwTexPrims[3] > 0)
            PF("Triple textured prims: %d (%f)", dwTexPrims[3], 100*dwTexPrims[3]/(float)dwDrawingPrimCount);
        if (dwTexPrims[4] > 0)
            PF("Quad textured prims: %d (%f)", dwTexPrims[4], 100*dwTexPrims[4]/(float)dwDrawingPrimCount);

        PF("Total state calls: %d, prims: %d, average: %f",
            dwStateCalls, dwStatePrimCount, dwStatePrimCount/((float)dwStateCalls));
        PF("Total uninterrupted drawing blocks: %d, average size: %f", dwDrawBlocks, dwDrawingPrimCount/(float)dwStateBlocks);
        PF("Total uninterrupted state blocks: %d, average size: %f", dwStateBlocks, dwStatePrimCount/(float)dwStateBlocks);
        PF("*************************************************************");  
    }

    if (dwFlags & OUTPUT_HWSTATE_CHANGES) { 
#if (NVARCH >= 0x020)
        PF("HW STATE BITS:");
        if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) {
            for (int i = 0; i < 8191; i++) 
            {
                if (dwStateBits[i] )
                {
                    DWORD state = i;
                    state <<= 16;
                    if (state & KELVIN_DIRTY_COMBINERS_COLOR)
                        PF("KELVIN_DIRTY_COMBINERS_COLOR |");
                    if (state & KELVIN_DIRTY_COMBINERS_SPECFOG)
                        PF("KELVIN_DIRTY_COMBINERS_SPECFOG |");
                    if (state & KELVIN_DIRTY_CONTROL0)
                        PF("KELVIN_DIRTY_CONTROL0 |");
                    if (state & KELVIN_DIRTY_DEFAULTS)
                        PF("KELVIN_DIRTY_DEFAULTS |");
                    if (state & KELVIN_DIRTY_FVF)
                        PF("KELVIN_DIRTY_FVF |");
                    if (state & KELVIN_DIRTY_LIGHTS)
                        PF("KELVIN_DIRTY_LIGHTS |");
                    if (state & KELVIN_DIRTY_MISC_STATE)
                        PF("KELVIN_DIRTY_MISC_STATE |");
                    if (state & KELVIN_DIRTY_SURFACE)
                        PF("KELVIN_DIRTY_SURFACE |");
                    if (state & KELVIN_DIRTY_TEXTURE_STATE)
                        PF("KELVIN_DIRTY_TEXTURE_STATE |");
                    if (state & KELVIN_DIRTY_TEXTURE_TRANSFORM)
                        PF("KELVIN_DIRTY_TEXTURE_TRANSFORM |");
                    if (state & KELVIN_DIRTY_TRANSFORM)
                        PF("KELVIN_DIRTY_TRANSFORM |");
                    if (state & KELVIN_DIRTY_VERTEX_SHADER)
                        PF("KELVIN_DIRTY_VERTEX_SHADER |");
                    if (state & KELVIN_DIRTY_PIXEL_SHADER)
                        PF("KELVIN_DIRTY_PIXEL_SHADER |");
                    PF("  %d", dwStateBits[i]); PF("");

                
                }
            }
        }
        if (dwStateBits[8191]) 
        {
            PF("KELVIN_DIRTY_REALLY_FILTHY: %d", dwStateBits[8191]);
        }
#endif

        PF("HW STATE CHANGES:");
        PF("Total HW state change pushbuffer size: %d, average per block: %f", 
            dwHWStateChangeSize, dwHWStateChangeSize/(float)dwStateBlocks); 
        PF("Total HW state changes: %d", dwHWStateChanges);
        PF("Color combiner state changes: %d", dwHWChanged[0]);
        PF("Specfog state changes: %d", dwHWChanged[1]);
        PF("Control0 state changes: %d", dwHWChanged[2]);
        PF("Defaults state changes: %d", dwHWChanged[3]);
        PF("FVF state changes: %d", dwHWChanged[4]);
        PF("Lights state changes: %d", dwHWChanged[5]);
        PF("Misc state changes: %d", dwHWChanged[6]);
        PF("Surface state changes: %d", dwHWChanged[7]);
        PF("Texture state changes: %d", dwHWChanged[8]);
        PF("Texture transform state changes: %d", dwHWChanged[9]);        
        PF("Transform state changes: %d", dwHWChanged[10]);
        PF("TL/Vertex shader state changes: %d", dwHWChanged[11]);
        PF("Pixel Shader state changes: %d", dwHWChanged[12]);
        PF("*************************************************************");
    }

    dwCurrentFrame = 0;

    if (dwFlags & CLEAR_DATA)
        resetData();

    return 0;
}


#endif