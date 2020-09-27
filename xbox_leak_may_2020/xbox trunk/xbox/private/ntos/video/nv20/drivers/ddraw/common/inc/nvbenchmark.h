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
// **************************************************************************
//
//  Module: nvBenchmark.h
//        Prototypes and definitions for nvBenchmark.cpp utility class
//
//  History:
//        Menelaos Levas    1/30/2001
//
// **************************************************************************

#ifdef NV_PROFILE_DP2OPS

#define OUTPUT_TOTALS                   0x80000000
#define OUTPUT_ALL_DP2OPS               0x40000000
#define OUTPUT_HWSTATE_CHANGES          0x20000000

#define CLEAR_DATA                      0x08000000
#define OUTPUT_FRAMECOUNT               0x0000FFFF



class CBenchmark
{
public:
    DWORD logData(DWORD dwCommand, DWORD dwPrimCount);
    DWORD logDrawPrimitive(DWORD dwCount);
    DWORD beginHWStateChange(DWORD dwFlags, DWORD dwPut);
    DWORD endHWStateChange(DWORD dwPut);
    DWORD incActiveTextures();
    DWORD resetData();
    DWORD outputData(DWORD dwFlags);

protected:
    DWORD dwActiveTextures, dwLastPrimCount;
    DWORD dwHWStateChangeSize, dwStartPut;
    DWORD dwStateBlocks, dwDrawBlocks, dwLastType;
    DWORD dwCurrentFrame, dwHWStateChanges;
    DWORD dwDrawingCalls, dwStateCalls;
    DWORD dwDrawingPrimCount, dwStatePrimCount;
    DWORD dwTexPrims[5];
    DWORD dwHWChanged[13];
    DWORD dwStateBits[8192];
    DWORD dwCallsCountTable[D3D_DP2OP_MAX+1];
    DWORD dwPrimsCountTable[D3D_DP2OP_MAX+1];
};

#endif