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
//  Module: nvKelvinComp.cpp
//      Kelvin inner loop compiler
//
// **************************************************************************
//
//  History:
//      Craig Duttweiler        29Jun2000         ported from celsius
//
// **************************************************************************
#include "nvprecomp.h"

#if (NVARCH >= 0x20)

#include "x86.h"
#include "nvILHash.h"

#ifdef KELVIN_ILC

//////////////////////////////////////////////////////////////////////////////
// notes:
//
// inner loop uniqueness is a function of:
//  - fvfData.dwVertexStride
//  - fvfData.dwVertexType
//  - fvfData.dwUVCount[8]
//  - fvfData.dwUVOffset[8]
//  - pContext->hwState.dwStateFlags & KELVIN_MASK_INNERLOOPUNIQUENESS
//  - pContext->hwState.kelvin.dwTexUnitToTexStageMapping[2]
//
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// switches
//
//#define PRINT_NAME          // prints ilcFlags for every primitive batch

//////////////////////////////////////////////////////////////////////////////
// aliases
//
#define ilcData         global.dwILCData
#define ilcCount        global.dwILCCount
#define ilcMax          global.dwILCMax

#define KNIMEM(x)           ((((DWORD)&((*(KATMAI_STATE*)global.kni).x[0])) + 15) & ~15)

//////////////////////////////////////////////////////////////////////////////
// external declarations (mostly nvcomp.cpp)
//
void ILCCompile_GetSpace (void);
void ILCCompile_mul      (DWORD reg,DWORD num);

// forward declarations
//
DWORD nvKelvinILCompile_inline_prim      (PNVD3DCONTEXT pContext,DWORD dwFlags);    // nvKelvinInlPrim.cpp
DWORD nvKelvinILCompile_inline_tri_list  (PNVD3DCONTEXT pContext,DWORD dwFlags);    // nvKelvinInlTri.cpp
DWORD nvKelvinILCompile_vb_prim          (PNVD3DCONTEXT pContext,DWORD dwFlags);    // nvKelvinVbPrim.cpp
// DWORD nvKelvinILCompile_str_prim         (PNVD3DCONTEXT pContext,DWORD dwFlags);    // nvKelvinInlStrPrim.cpp
DWORD nvKelvinILCompile_super_tri_list   (PNVD3DCONTEXT pContext,DWORD dwFlags);    // nvKelvinSuperTri.cpp

//---------------------------------------------------------------------------

// nvKelvinGetDispatchRoutine

KELVINDISPATCHPRIMITIVE nvKelvinGetDispatchRoutine
(
    PNVD3DCONTEXT pContext,
    DWORD         dwFlags
)
{
    // check quick cache
    if (global.kelvin.dwLoopCache)
    {
        CILHashEntry *pEntry = (CILHashEntry*)global.kelvin.dwLoopCache;
        if (pEntry->match (pContext,NVCLASS_FAMILY_KELVIN,dwFlags))
        {
            return (KELVINDISPATCHPRIMITIVE)(ilcData + pEntry->getOffset());
        }
    }

    // check hash table
    DWORD dwHashIndex = (DRAW_PRIM_TABLE_ENTRIES - 1) & ((dwFlags + (dwFlags >> 12) + (dwFlags >> 24))
                                                      + pContext->hwState.pVertexShader->getStride()
                                                      + pContext->hwState.pVertexShader->getFVF());

    CILHashEntry *pEntry = (CILHashEntry*)dwDrawPrimitiveTable[dwHashIndex];

    while (pEntry)
    {
        // found it?
        if (pEntry->match (pContext,NVCLASS_FAMILY_KELVIN,dwFlags))
        {
            global.kelvin.dwLoopCache = (DWORD)pEntry;
            return (KELVINDISPATCHPRIMITIVE)(ilcData + pEntry->getOffset());
        }
        // next
        pEntry = pEntry->getNext();
    }

    // build new innerloop
    //  loops require pContext->hwState.dwStateFlags to be set up correctly (CM hack, inline expansion)
    DWORD dwOffset;
    switch (dwFlags & KELVIN_ILMASK_LOOPTYPE)
    {
        case KELVIN_ILFLAG_IX_VB_PRIM:
        case KELVIN_ILFLAG_OR_VB_PRIM:       dwOffset = nvKelvinILCompile_vb_prim(pContext,dwFlags);
                                             break;
        case KELVIN_ILFLAG_IX_STR_PRIM:
        case KELVIN_ILFLAG_OR_STR_PRIM:      //dwOffset = nvKelvinILCompile_str_prim(pContext,dwFlags);
                                             dwOffset = 0;
                                             break;
        case KELVIN_ILFLAG_IX_INL_PRIM:
        case KELVIN_ILFLAG_OR_INL_PRIM:      dwOffset = nvKelvinILCompile_inline_prim(pContext,dwFlags);
                                             break;
        case KELVIN_ILFLAG_IX_INL_TRI_LIST:
        case KELVIN_ILFLAG_OR_INL_TRI_LIST:  dwOffset = nvKelvinILCompile_inline_tri_list(pContext,dwFlags);
                                             break;
        case KELVIN_ILFLAG_SUPER_TRI_LIST:   dwOffset = nvKelvinILCompile_super_tri_list(pContext,dwFlags);
                                             break;
        case KELVIN_ILFLAG_IX_DVB_TRI:       dwOffset = 0;//todo - nvKelvinILCompile_indexed_dvb_tri(pContext,dwFlags);
                                             break;
    }

    // add to hash list
    pEntry = new CILHashEntry (pContext,NVCLASS_FAMILY_KELVIN,dwFlags,dwOffset,(CILHashEntry*)dwDrawPrimitiveTable[dwHashIndex]);
    if (!pEntry) return NULL;
    dwDrawPrimitiveTable[dwHashIndex] = (DWORD)pEntry;

#ifdef PRINT_NAME
    PF ("new kelvin loop (%08x): type %x, dp2 %s (%2x), state = %08x, %s %s", pEntry,
        (dwFlags & 0xF0000000) >> 28,
        kelvinPrimitiveName[dwFlags & 0xFFFF], dwFlags & 0xFFFF,
        pContext->hwState.dwStateFlags & KELVIN_MASK_INNERLOOPUNIQUENESS,
        dwFlags & KELVIN_ILFLAG_LEGACY ? "LEGACY" : "",
        dwFlags & KELVIN_ILFLAG_NOCULL ? "NOCULL" : "");
#endif

    // done
    global.kelvin.dwLoopCache = (DWORD)pEntry;
    return (KELVINDISPATCHPRIMITIVE)(ilcData + pEntry->getOffset());
}


//---------------------------------------------------------------------------

// helpers

void __stdcall nvKelvinDispatchGetPusherSpace (void)
{
    // wrap around -or- make space
    nvPusherAdjust (0);
}

//---------------------------------------------------------------------------

#ifdef DEBUG
void __stdcall nvKelvinDispatchFlush (void)
{
    nvPusherAdjust (0);
    getDC()->nvPusher.flush (TRUE, CPushBuffer::FLUSH_WITH_DELAY);
}

void __stdcall nvKelvinDispatchPrintVertex (PNVD3DCONTEXT pContext,DWORD dwPutAddress)
{
    static DWORD  dwVertexCount = 0;
           DWORD *pdwData       = ((DWORD*)dwPutAddress) + 1;

    if (dbgShowState & NVDBG_SHOW_VERTICES)
    {
        dwVertexCount ++;

        DPF ("Vertex %d",dwVertexCount);

        if (pContext->hwState.pVertexShader->bVAExists(defaultInputRegMap[D3DVSDE_BLENDWEIGHT])) {
            DPF ("         B = %08x",pdwData[0]);
            pdwData += 1;
        }

        if (pContext->hwState.pVertexShader->bVAExists(defaultInputRegMap[D3DVSDE_NORMAL])) {
            DPF ("    Normal = %f, %f, %f", FLOAT_FROM_DWORD(pdwData[0]),FLOAT_FROM_DWORD(pdwData[1]),FLOAT_FROM_DWORD(pdwData[2]));
            DPF ("           = [%08x, %08x, %08x]", pdwData[0],pdwData[1],pdwData[2]);
            pdwData += 3;
        }

        if (pContext->hwState.dwStateFlags & KELVIN_FLAG_USERTEXCOORDSNEEDED(1)) {
            DWORD dwD3DStage = pContext->hwState.dwTexUnitToTexStageMapping[1];
            DWORD dwTCIndex  = (pContext->hwState.dwTexCoordIndices >> 8) & 0xff;
            DWORD dwVAIndex  = defaultInputRegMap[D3DVSDE_TEXCOORD0+dwTCIndex];
            DWORD dwCount    = (pContext->hwState.pVertexShader->getVASize(dwVAIndex) >> 2);
            for (DWORD i = 0; i < dwCount; i++)
            {
                DPF ("   UV1[%d] = %f [%08x]", i,FLOAT_FROM_DWORD(pdwData[0]),pdwData[0]);
                pdwData += 1;
            }
        }

        if (pContext->hwState.dwStateFlags & KELVIN_FLAG_USERTEXCOORDSNEEDED(0)) {
            DWORD dwD3DStage = pContext->hwState.dwTexUnitToTexStageMapping[0];
            DWORD dwTCIndex  = (pContext->hwState.dwTexCoordIndices >> 0) & 0xff;
            DWORD dwVAIndex  = defaultInputRegMap[D3DVSDE_TEXCOORD0+dwTCIndex];
            DWORD dwCount    = (pContext->hwState.pVertexShader->getVASize(dwVAIndex) >> 2);
            for (DWORD i = 0; i < dwCount; i++)
            {
                DPF ("   UV0[%d] = %f [%08x]", i,FLOAT_FROM_DWORD(pdwData[0]),pdwData[0]);
                pdwData += 1;
            }
        }

        if (pContext->hwState.pVertexShader->bVAExists(defaultInputRegMap[D3DVSDE_SPECULAR])){
            DPF ("  Specular = %08x",pdwData[0]);
            pdwData += 1;
        }

        if (pContext->hwState.pVertexShader->bVAExists(defaultInputRegMap[D3DVSDE_DIFFUSE])) {
            DPF ("   Diffuse = %08x",pdwData[0]);
            pdwData += 1;
        }

        nvAssert (pContext->hwState.pVertexShader->bVAExists(defaultInputRegMap[D3DVSDE_POSITION]));
        DPF ("       XYZ = %f, %f, %f", FLOAT_FROM_DWORD(pdwData[0]),FLOAT_FROM_DWORD(pdwData[1]),FLOAT_FROM_DWORD(pdwData[2]));
        DPF ("           = [%08x, %08x, %08x]", pdwData[0],pdwData[1],pdwData[2]);
        pdwData += 3;

        if (pContext->hwState.pVertexShader->getVASize(defaultInputRegMap[D3DVSDE_POSITION]) == 4*sizeof(DWORD)) {
            DPF ("       RHW = %f [%08x]", FLOAT_FROM_DWORD(pdwData[0]),pdwData[0]);
            pdwData += 1;
        }
        DPF ("");
    }
}
#endif

// copies dwCount bytes from [esi] to [edi] with increment. if dwCount is zero then we copy ecx bytes
// we use esi,edi,eax and ecx. for kni we also use xmm0 to xmm3
void nvKelvinILCompile_memcpy
(
    DWORD dwCount, bool recurse
)
{

//    if (dwCount)
//    {
//        xMOV_rm_imm (rmREG(rECX),dwCount)
//    }
//
//    xMOV_r_rm   (rEAX,rmREG(rECX))
//    xSHR_rm_imm (rmREG(rECX),2)
//    xAND_rm_imm (rmREG(rEAX),3)
//    xREP xMOVSD
//    xMOV_r_rm   (rECX,rmREG(rEAX))
//    xREP xMOVSB
//
    if (dwCount)
    {
        // do copy
        if (pDriverData->nvD3DPerfData.dwCPUFeatureSet & FS_KATMAI)
        {
            //
            // KNI copy
            //
            while (dwCount >= 64)
            {
                xMOVLPS_r_rm    (rXMM0,rmIND(rESI))
                xMOVHPS_r_rm    (rXMM0,rmIND8(rESI)) xOFS8(8)
                xMOVLPS_r_rm    (rXMM1,rmIND8(rESI)) xOFS8(16)
                xMOVHPS_r_rm    (rXMM1,rmIND8(rESI)) xOFS8(24)
                xMOVLPS_r_rm    (rXMM2,rmIND8(rESI)) xOFS8(32)
                xMOVHPS_r_rm    (rXMM2,rmIND8(rESI)) xOFS8(40)
                xMOVLPS_r_rm    (rXMM3,rmIND8(rESI)) xOFS8(48)
                xMOVHPS_r_rm    (rXMM3,rmIND8(rESI)) xOFS8(56)
                xADD_rm_imm     (rmREG(rESI),64)
                xMOVLPS_rm_r    (rmIND(rEDI),rXMM0)
                xMOVHPS_rm_r    (rmIND8(rEDI),rXMM0) xOFS8(8)
                xMOVLPS_rm_r    (rmIND8(rEDI),rXMM1) xOFS8(16)
                xMOVHPS_rm_r    (rmIND8(rEDI),rXMM1) xOFS8(24)
                xMOVLPS_rm_r    (rmIND8(rEDI),rXMM2) xOFS8(32)
                xMOVHPS_rm_r    (rmIND8(rEDI),rXMM2) xOFS8(40)
                xMOVLPS_rm_r    (rmIND8(rEDI),rXMM3) xOFS8(48)
                xMOVHPS_rm_r    (rmIND8(rEDI),rXMM3) xOFS8(56)
                xADD_rm_imm     (rmREG(rEDI),64)
                dwCount -= 64;
            }

            while (dwCount >= 16)
            {
                xMOVLPS_r_rm    (rXMM0,rmIND(rESI))
                xMOVHPS_r_rm    (rXMM0,rmIND8(rESI)) xOFS8(8)
                xADD_rm_imm     (rmREG(rESI),16)
                xMOVLPS_rm_r    (rmIND(rEDI),rXMM0)
                xMOVHPS_rm_r    (rmIND8(rEDI),rXMM0) xOFS8(8)
                xADD_rm_imm     (rmREG(rEDI),16)
                dwCount -= 16;
            }

            while (dwCount >= 8)
            {
                xMOV_r_rm    (rEAX,rmIND(rESI))
                xMOV_r_rm    (rECX,rmIND8(rESI)) xOFS8(4)
                xADD_rm_imm  (rmREG(rESI),8)
                xMOV_rm_r    (rmIND(rEDI),rEAX)
                xADD_rm_imm  (rmREG(rEDI),8)
                xMOV_rm_r    (rmIND8(rEDI),rECX) xOFS8(4 - 8)
                dwCount -= 8;
            }

            while (dwCount >= 4)
            {
                xMOV_r_rm    (rEAX,rmIND(rESI))
                xADD_rm_imm  (rmREG(rESI),4)
                xMOV_rm_r    (rmIND(rEDI),rEAX)
                xADD_rm_imm  (rmREG(rEDI),4)
                dwCount -= 4;
            }

            if (dwCount)
            {
                xMOV_rm_imm (rmREG(rECX),dwCount)
                xREP xMOVSB
            }
        }
        else if (pDriverData->nvD3DPerfData.dwCPUFeatureSet & FS_ATHLON)
        {
            //
            // AMD copy
            //
            bool needFemms = false;
            if (dwCount >= 16 && !recurse)
            {
                xFEMMS
                needFemms = true;
            }

            while (dwCount >= 64)
            {
                xMOVQ_r_rm      (rMM0,rmIND(rESI))
                xMOVQ_r_rm      (rMM1,rmIND8(rESI)) xOFS8(8)
                xMOVQ_r_rm      (rMM2,rmIND8(rESI)) xOFS8(16)
                xMOVQ_r_rm      (rMM3,rmIND8(rESI)) xOFS8(24)
                xMOVQ_r_rm      (rMM4,rmIND8(rESI)) xOFS8(32)
                xMOVQ_r_rm      (rMM5,rmIND8(rESI)) xOFS8(40)
                xMOVQ_r_rm      (rMM6,rmIND8(rESI)) xOFS8(48)
                xMOVQ_r_rm      (rMM7,rmIND8(rESI)) xOFS8(56)
                xADD_rm_imm     (rmREG(rESI),64)
                xMOVNTQ_rm_r    (rmIND(rEDI),rMM0)
                xMOVNTQ_rm_r    (rmIND8(rEDI),rMM1) xOFS8(8)
                xMOVNTQ_rm_r    (rmIND8(rEDI),rMM2) xOFS8(16)
                xMOVNTQ_rm_r    (rmIND8(rEDI),rMM3) xOFS8(24)
                xMOVNTQ_rm_r    (rmIND8(rEDI),rMM4) xOFS8(32)
                xMOVNTQ_rm_r    (rmIND8(rEDI),rMM5) xOFS8(40)
                xMOVNTQ_rm_r    (rmIND8(rEDI),rMM6) xOFS8(48)
                xMOVNTQ_rm_r    (rmIND8(rEDI),rMM7) xOFS8(56)
                xADD_rm_imm     (rmREG(rEDI),64)
                dwCount -= 64;
            }

            while (dwCount >= 16)
            {
                xMOVQ_r_rm      (rMM0,rmIND(rESI))
                xMOVQ_r_rm      (rMM1,rmIND8(rESI)) xOFS8(8)
                xADD_rm_imm     (rmREG(rESI),16)
                xMOVNTQ_rm_r    (rmIND(rEDI),rMM0)
                xMOVNTQ_rm_r    (rmIND8(rEDI),rMM1) xOFS8(8)
                xADD_rm_imm     (rmREG(rEDI),16)
                dwCount -= 16;
            }

            while (dwCount >= 8)
            {
                xMOV_r_rm    (rEAX,rmIND(rESI))
                xMOV_r_rm    (rECX,rmIND8(rESI)) xOFS8(4)
                xADD_rm_imm  (rmREG(rESI),8)
                xMOV_rm_r    (rmIND(rEDI),rEAX)
                xADD_rm_imm  (rmREG(rEDI),8)
                xMOV_rm_r    (rmIND8(rEDI),rECX) xOFS8(4 - 8)
                dwCount -= 8;
            }

            while (dwCount >= 4)
            {
                xMOV_r_rm    (rEAX,rmIND(rESI))
                xADD_rm_imm  (rmREG(rESI),4)
                xMOV_rm_r    (rmIND(rEDI),rEAX)
                xADD_rm_imm  (rmREG(rEDI),4)
                dwCount -= 4;
            }

            if (dwCount)
            {
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
    }
    else
    {
        // variable length copy
        if ((pDriverData->nvD3DPerfData.dwCPUFeatureSet & FS_KATMAI) || (pDriverData->nvD3DPerfData.dwCPUFeatureSet & FS_ATHLON))
        {
            bool needFemms = false;
            if (dwCount >= 16 && !recurse)
            {
                xFEMMS
                needFemms = true;
            }

            //
            // kni copy
            //
            DWORD label1;
            DWORD label2;
            // 64 byte xfers
            xLABEL      (label2)
            xCMP_rm_imm (rmREG(rECX),64)
            xLABEL      (label1)
            xJL32       (0)
            xPUSH_r     (rECX)
            nvKelvinILCompile_memcpy (64, true);
            xPOP_r      (rECX)
            xLEA_r_rm   (rECX,rmIND8(rECX)) xOFS8(-64)
            xJMP        (label2)
            xTARGET_b32 (label1)
            // 16 byte xfers
            xLABEL      (label2)
            xCMP_rm_imm (rmREG(rECX),16)
            xLABEL      (label1)
            xJL32       (0)
            xPUSH_r     (rECX)
            nvKelvinILCompile_memcpy (16, true);
            xPOP_r      (rECX)
            xLEA_r_rm   (rECX,rmIND8(rECX)) xOFS8(-16)
            xJMP        (label2)
            xTARGET_b32 (label1)
            // the rest
            xMOV_r_rm   (rEAX,rmREG(rECX))
            xSHR_rm_imm8(rmREG(rECX),2)
            xAND_rm_imm (rmREG(rEAX),3)
            xREP xMOVSD
            xMOV_r_rm   (rECX,rmREG(rEAX))
            xREP xMOVSB

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
            xMOV_r_rm   (rEAX,rmREG(rECX))
            xSHR_rm_imm8(rmREG(rECX),2)
            xAND_rm_imm (rmREG(rEAX),3)
            xREP xMOVSD
            xMOV_r_rm   (rECX,rmREG(rEAX))
            xREP xMOVSB
        }
    }
}

// adds a base vertex index (16bit) to all 16bit chunks
// copies dwCount bytes from [esi] to [edi] with increment. if dwCount is zero then we copy ecx bytes
// we use esi,edi,eax and ecx. for kni we also use xmm0 to xmm3
void nvKelvinILCompile_indexcpy
(
    DWORD dwCount
)
{
    if (dwCount)
    {
        // do copy
        if (pDriverData->nvD3DPerfData.dwCPUFeatureSet & FS_WILLAMETTE)
        {
            //
            // WNI copy
            //
            xMOV_r_i32      (rECX,mMEM32(global.kelvin.dwBaseVertex))
            xSHL_rm_imm8    (rmREG(rECX),16)
            xOR_r_i32       (rECX,mMEM32(global.kelvin.dwBaseVertex))
            xMOVDX_r_rm      (rXMM4, rmREG(rECX))
            xPSHUFLW_r_rm_imm (rXMM4, rmREG(rXMM4), 0)
            xPUNPCKLWDX_r_rm (rXMM4, rmREG(rXMM4))

            while (dwCount >= 64)
            {
                xMOVLPS_r_rm    (rXMM0,rmIND(rESI))
                xMOVHPS_r_rm    (rXMM0,rmIND8(rESI)) xOFS8(8)
                xPADDWX_r_rm    (rXMM0, rmREG(rXMM4))
                xMOVLPS_r_rm    (rXMM1,rmIND8(rESI)) xOFS8(16)
                xMOVHPS_r_rm    (rXMM1,rmIND8(rESI)) xOFS8(24)
                xPADDWX_r_rm    (rXMM1, rmREG(rXMM4))
                xMOVLPS_r_rm    (rXMM2,rmIND8(rESI)) xOFS8(32)
                xMOVHPS_r_rm    (rXMM2,rmIND8(rESI)) xOFS8(40)
                xPADDWX_r_rm    (rXMM2, rmREG(rXMM4))
                xMOVLPS_r_rm    (rXMM3,rmIND8(rESI)) xOFS8(48)
                xMOVHPS_r_rm    (rXMM3,rmIND8(rESI)) xOFS8(56)
                xPADDWX_r_rm    (rXMM3, rmREG(rXMM4))
                xADD_rm_imm     (rmREG(rESI),64)
                xMOVLPS_rm_r    (rmIND(rEDI),rXMM0)
                xMOVHPS_rm_r    (rmIND8(rEDI),rXMM0) xOFS8(8)
                xMOVLPS_rm_r    (rmIND8(rEDI),rXMM1) xOFS8(16)
                xMOVHPS_rm_r    (rmIND8(rEDI),rXMM1) xOFS8(24)
                xMOVLPS_rm_r    (rmIND8(rEDI),rXMM2) xOFS8(32)
                xMOVHPS_rm_r    (rmIND8(rEDI),rXMM2) xOFS8(40)
                xMOVLPS_rm_r    (rmIND8(rEDI),rXMM3) xOFS8(48)
                xMOVHPS_rm_r    (rmIND8(rEDI),rXMM3) xOFS8(56)
                xADD_rm_imm     (rmREG(rEDI),64)
                dwCount -= 64;
            }

            while (dwCount >= 16)
            {
                xMOVLPS_r_rm    (rXMM0,rmIND(rESI))
                xMOVHPS_r_rm    (rXMM0,rmIND8(rESI)) xOFS8(8)
                xPADDWX_r_rm    (rXMM0, rmREG(rXMM4))
                xADD_rm_imm     (rmREG(rESI),16)
                xMOVLPS_rm_r    (rmIND(rEDI),rXMM0)
                xMOVHPS_rm_r    (rmIND8(rEDI),rXMM0) xOFS8(8)
                xADD_rm_imm     (rmREG(rEDI),16)
                dwCount -= 16;
            }
        }
        else if (pDriverData->nvD3DPerfData.dwCPUFeatureSet & FS_KATMAI)
        {
            //
            // KNI copy - we use MMX because there's no integer add for KNI regs.
            //
            xEMMS
            xMOV_r_i32      (rECX,mMEM32(global.kelvin.dwBaseVertex))
            xSHL_rm_imm8    (rmREG(rECX),16)
            xOR_r_i32       (rECX,mMEM32(global.kelvin.dwBaseVertex))
            xMOVD_r_rm      (rMM4, rmREG(rECX))
            xPSHUFW_r_rm_imm (rMM4, rmREG(rMM4), 0)
            while (dwCount >= 64)
            {

                xMOVQ_r_rm      (rMM0,rmIND(rESI))
                xPADDW_r_rm     (rMM0, rmREG(rMM4))
                xMOVQ_r_rm      (rMM1,rmIND8(rESI)) xOFS8(8)
                xPADDW_r_rm     (rMM1, rmREG(rMM4))
                xMOVQ_r_rm      (rMM2,rmIND8(rESI)) xOFS8(16)
                xPADDW_r_rm     (rMM2, rmREG(rMM4))
                xMOVQ_r_rm      (rMM3,rmIND8(rESI)) xOFS8(24)
                xPADDW_r_rm     (rMM3, rmREG(rMM4))

                xMOVNTQ_rm_r    (rmIND(rEDI),rMM0)
                xMOVNTQ_rm_r    (rmIND8(rEDI),rMM1) xOFS8(8)
                xMOVNTQ_rm_r    (rmIND8(rEDI),rMM2) xOFS8(16)
                xMOVNTQ_rm_r    (rmIND8(rEDI),rMM3) xOFS8(24)

                xMOVQ_r_rm      (rMM0,rmIND8(rESI)) xOFS8(32)
                xPADDW_r_rm     (rMM0, rmREG(rMM4))
                xMOVQ_r_rm      (rMM1,rmIND8(rESI)) xOFS8(40)
                xPADDW_r_rm     (rMM1, rmREG(rMM4))
                xMOVQ_r_rm      (rMM2,rmIND8(rESI)) xOFS8(48)
                xPADDW_r_rm     (rMM2, rmREG(rMM4))
                xMOVQ_r_rm      (rMM3,rmIND8(rESI)) xOFS8(56)
                xPADDW_r_rm     (rMM3, rmREG(rMM4))

                xADD_rm_imm     (rmREG(rESI),64)


                xMOVNTQ_rm_r    (rmIND8(rEDI),rMM0) xOFS8(32)
                xMOVNTQ_rm_r    (rmIND8(rEDI),rMM1) xOFS8(40)
                xMOVNTQ_rm_r    (rmIND8(rEDI),rMM2) xOFS8(48)
                xMOVNTQ_rm_r    (rmIND8(rEDI),rMM3) xOFS8(56)
                xADD_rm_imm     (rmREG(rEDI),64)


                dwCount -= 64;
            }

            while (dwCount >= 16)
            {
                xMOVQ_r_rm      (rMM0,rmIND(rESI))
                xPADDW_r_rm     (rMM0, rmREG(rMM4))
                xMOVQ_r_rm      (rMM1,rmIND8(rESI)) xOFS8(8)
                xPADDW_r_rm     (rMM1, rmREG(rMM4))
                xADD_rm_imm     (rmREG(rESI),16)
                xMOVNTQ_rm_r    (rmIND(rEDI),rMM0)
                xMOVNTQ_rm_r    (rmIND8(rEDI),rMM1) xOFS8(8)
                xADD_rm_imm     (rmREG(rEDI),16)
                dwCount -= 16;
            }
            xEMMS

        }
        else if (pDriverData->nvD3DPerfData.dwCPUFeatureSet & FS_ATHLON)
        {
            //
            // AMD copy
            //
            xFEMMS
            xMOV_r_i32      (rECX,mMEM32(global.kelvin.dwBaseVertex))
            xSHL_rm_imm8    (rmREG(rECX),16)
            xOR_r_i32       (rECX,mMEM32(global.kelvin.dwBaseVertex))
            xMOVD_r_rm      (rMM4, rmREG(rECX))
            xPUNPCKLDQ_r_rm (rMM4, rmREG(rMM4))
            while (dwCount >= 64)
            {
                xMOVQ_r_rm      (rMM0,rmIND(rESI))
                xPADDW_r_rm     (rMM0, rmREG(rMM4))
                xMOVQ_r_rm      (rMM1,rmIND8(rESI)) xOFS8(8)
                xPADDW_r_rm     (rMM1, rmREG(rMM4))
                xMOVQ_r_rm      (rMM2,rmIND8(rESI)) xOFS8(16)
                xPADDW_r_rm     (rMM2, rmREG(rMM4))
                xMOVQ_r_rm      (rMM3,rmIND8(rESI)) xOFS8(24)
                xPADDW_r_rm     (rMM3, rmREG(rMM4))

                xMOVNTQ_rm_r    (rmIND(rEDI),rMM0)
                xMOVNTQ_rm_r    (rmIND8(rEDI),rMM1) xOFS8(8)
                xMOVNTQ_rm_r    (rmIND8(rEDI),rMM2) xOFS8(16)
                xMOVNTQ_rm_r    (rmIND8(rEDI),rMM3) xOFS8(24)

                xMOVQ_r_rm      (rMM0,rmIND8(rESI)) xOFS8(32)
                xPADDW_r_rm     (rMM0, rmREG(rMM4))
                xMOVQ_r_rm      (rMM1,rmIND8(rESI)) xOFS8(40)
                xPADDW_r_rm     (rMM1, rmREG(rMM4))
                xMOVQ_r_rm      (rMM2,rmIND8(rESI)) xOFS8(48)
                xPADDW_r_rm     (rMM2, rmREG(rMM4))
                xMOVQ_r_rm      (rMM3,rmIND8(rESI)) xOFS8(56)
                xPADDW_r_rm     (rMM3, rmREG(rMM4))

                xADD_rm_imm     (rmREG(rESI),64)


                xMOVNTQ_rm_r    (rmIND8(rEDI),rMM0) xOFS8(32)
                xMOVNTQ_rm_r    (rmIND8(rEDI),rMM1) xOFS8(40)
                xMOVNTQ_rm_r    (rmIND8(rEDI),rMM2) xOFS8(48)
                xMOVNTQ_rm_r    (rmIND8(rEDI),rMM3) xOFS8(56)
                xADD_rm_imm     (rmREG(rEDI),64)


                dwCount -= 64;
            }

            while (dwCount >= 16)
            {
                xMOVQ_r_rm      (rMM0,rmIND(rESI))
                xPADDW_r_rm     (rMM0, rmREG(rMM4))
                xMOVQ_r_rm      (rMM1,rmIND8(rESI)) xOFS8(8)
                xPADDW_r_rm     (rMM1, rmREG(rMM4))
                xADD_rm_imm     (rmREG(rESI),16)
                xMOVNTQ_rm_r    (rmIND(rEDI),rMM0)
                xMOVNTQ_rm_r    (rmIND8(rEDI),rMM1) xOFS8(8)
                xADD_rm_imm     (rmREG(rEDI),16)
                dwCount -= 16;
            }
            xFEMMS
        }
        if (dwCount)
        {
            //
            // normal 8086 copy
            //
            DWORD label;
            xLABEL (label)


            xMOV_r_i32  (rECX,mMEM32(global.kelvin.dwBaseVertex))
            xSHL_rm_imm8 (rmREG(rECX),16)
            xOR_r_i32   (rECX,mMEM32(global.kelvin.dwBaseVertex))
            //hmmm fully unrolled loop might not be the best... HMH
            //never mind, 64 indices is max (aka 32 moves)
            while (dwCount >= 4)
            {
                xMOV_r_rm    (rEAX,rmIND(rESI))
                xADD_rm_imm  (rmREG(rESI),4)
                xADD_r_rm    (rEAX, rmREG(rECX)) //add base index
                xMOV_rm_r    (rmIND(rEDI),rEAX)
                xADD_rm_imm  (rmREG(rEDI),4)
                dwCount -= 4;

            }
            if (dwCount) //it had better be 2
            {
                x16r xMOV_r_rm (rAX,rmIND(rESI))
                xADD_rm_imm  (rmREG(rESI),2)
                xADD_r_rm    (rEAX, rmREG(rECX)) //add base index
                xMOV_rm_r    (rmIND(rEDI),rAX)
                xADD_rm_imm  (rmREG(rEDI),2)
                dwCount -= 2;
            }
        }
    }
}


// vertex at ESI, pusher at EDI
//  thrashes eax,ebx,ecx,edx and ebp
void nvKelvinILCompile_copyVertex
(
    PNVD3DCONTEXT pContext,
    DWORD         dwFlags
)
{
    CVertexShader *pVShader;

    const DWORD    adwRegister[]        = { rEAX,rEBX,rECX,rEDX,rEBP };
    DWORD          dwDstIndex           = 0;
    DWORD          dwInlineVertexStride = pContext->hwState.dwInlineVertexStride;

    // cache the vertex shader
    pVShader = pContext->hwState.pVertexShader;
    nvAssert (pVShader);

    // rd: method
    xMOV_rm_imm (rmREG(rEAX),(((dwInlineVertexStride) << 16) | ((NV_DD_KELVIN) << 13) | (NVPUSHER_NOINC(NV097_INLINE_ARRAY))))

    // wr: method
    xMOV_rm_r   (rmIND(rEDI),rEAX)
    dwDstIndex += 4;

    nvAssert (pVShader->bVAExists(defaultInputRegMap[D3DVSDE_POSITION]));

    // rd: xyz
    xMOV_r_rm   (rECX,rmIND8(rESI)) xOFS8(global.kelvin.dwOffsetXYZ)
    xMOV_r_rm   (rEDX,rmIND8(rESI)) xOFS8(global.kelvin.dwOffsetXYZ + 4)
    xMOV_r_rm   (rEBP,rmIND8(rESI)) xOFS8(global.kelvin.dwOffsetXYZ + 8)

    // wr: xyz
    xMOV_rm_r   (rmIND8(rEDI),rECX) xOFS8(dwDstIndex)
    xMOV_rm_r   (rmIND8(rEDI),rEDX) xOFS8(dwDstIndex + 4)
    xMOV_rm_r   (rmIND8(rEDI),rEBP) xOFS8(dwDstIndex + 8)
    dwDstIndex += 12;

    // rd & wr: rhw
    if (pVShader->getVASize(defaultInputRegMap[D3DVSDE_POSITION]) == 4*sizeof(DWORD))
    {
        xMOV_r_rm   (rEAX,rmIND8(rESI)) xOFS8(global.kelvin.dwOffsetRHW)
        xMOV_rm_r   (rmIND8(rEDI),rEAX) xOFS8(dwDstIndex)
        dwDstIndex += 4;
    }

    // rd & wr: blend weights
    if (pVShader->bVAExists(defaultInputRegMap[D3DVSDE_BLENDWEIGHT]))
    {
        DWORD dwNumWeights = pVShader->getVASize(defaultInputRegMap[D3DVSDE_BLENDWEIGHT]) >> 2;
        for (DWORD i=0; i<dwNumWeights; i++) {
            xMOV_r_rm   (rEBX,rmIND8(rESI)) xOFS8(global.kelvin.dwOffsetWeight + 4*i)
            xMOV_rm_r   (rmIND8(rEDI),rEBX) xOFS8(dwDstIndex)
            dwDstIndex += 4;
        }
    }

    // rd & wr: normal
    if (pVShader->bVAExists(defaultInputRegMap[D3DVSDE_NORMAL]))
    {
        xMOV_r_rm   (rECX,rmIND8(rESI)) xOFS8(global.kelvin.dwOffsetNormal)
        xMOV_r_rm   (rEDX,rmIND8(rESI)) xOFS8(global.kelvin.dwOffsetNormal + 4)
        xMOV_r_rm   (rEBP,rmIND8(rESI)) xOFS8(global.kelvin.dwOffsetNormal + 8)
        xMOV_rm_r   (rmIND8(rEDI),rECX) xOFS8(dwDstIndex)
        xMOV_rm_r   (rmIND8(rEDI),rEDX) xOFS8(dwDstIndex + 4)
        xMOV_rm_r   (rmIND8(rEDI),rEBP) xOFS8(dwDstIndex + 8)
        dwDstIndex += 12;
    }

    // rd: diffuse
    if (pVShader->bVAExists(defaultInputRegMap[D3DVSDE_DIFFUSE]))
    {
        xMOV_r_rm   (rEAX,rmIND8(rESI)) xOFS8(global.kelvin.dwOffsetDiffuse)
    }

    // rd: specular
    if (pVShader->bVAExists(defaultInputRegMap[D3DVSDE_SPECULAR]))
    {
        xMOV_r_rm   (rEBX,rmIND8(rESI)) xOFS8(global.kelvin.dwOffsetSpecular)
    }

    // rd: point size
    if (pVShader->bVAExists(defaultInputRegMap[D3DVSDE_PSIZE]))
    {
        xMOV_r_rm   (rECX,rmIND8(rESI)) xOFS8(global.kelvin.dwOffsetPointSize)
    }

    // wr: diffuse
    if (pVShader->bVAExists(defaultInputRegMap[D3DVSDE_DIFFUSE]))
    {
        xMOV_rm_r   (rmIND8(rEDI),rEAX) xOFS8(dwDstIndex)
        dwDstIndex += 4;
    }

    // wr: specular
    if (pVShader->bVAExists(defaultInputRegMap[D3DVSDE_SPECULAR]))
    {
        xMOV_rm_r   (rmIND8(rEDI),rEBX) xOFS8(dwDstIndex)
        dwDstIndex += 4;
    }

    // wr: point size
    if (pVShader->bVAExists(defaultInputRegMap[D3DVSDE_PSIZE]))
    {
        xMOV_rm_r   (rmIND8(rEDI),rECX) xOFS8(dwDstIndex)
        dwDstIndex += 4;
    }

    for (DWORD dwHWStage = 0; dwHWStage < KELVIN_NUM_TEXTURES; dwHWStage++)
    {
        if (pContext->hwState.dwStateFlags & KELVIN_FLAG_USERTEXCOORDSNEEDED(dwHWStage))
        {
            DWORD dwD3DStage = pContext->hwState.dwTexUnitToTexStageMapping[dwHWStage];
            DWORD dwTCIndex  = (pContext->hwState.dwTexCoordIndices >> (8*dwHWStage)) & 0xff;  // BUGBUG?? are these indices even relevant in DX8?
            DWORD dwVAIndex  = defaultInputRegMap[D3DVSDE_TEXCOORD0+dwTCIndex];
            DWORD dwCount    = ((pVShader->getVASize (dwVAIndex)) >> 2);
            assert (dwCount < 5);
            {
                for (DWORD i = 0; i < dwCount; i++)
                {
                    xMOV_r_rm   (adwRegister[i],rmIND8(rESI)) xOFS8(i * 4 + global.kelvin.dwOffsetUV[dwTCIndex])
                }

#ifdef STOMP_TEX_COORDS
                if (pContext->hwState.dwStateFlags & KELVIN_FLAG_STOMP_4TH_COORD(dwHWStage))
                {
                    xMOV_r_imm  (adwRegister[3],0x3f800000)
                }
#endif  // STOMP_TEX_COORDS

                for (DWORD k = 0; k < dwCount; k++)
                {
                    xMOV_rm_r   (rmIND8(rEDI),adwRegister[k]) xOFS8(dwDstIndex)
                    dwDstIndex += 4;
                }
            }
        }
    }

    // sanity check
    assert ((dwDstIndex - 4) == dwInlineVertexStride);

    // we can optionally print the vertex here
#if defined(DEBUG) || defined(NVSTATDRIVER)
    xMOV_rm_imm (rmREG(rEAX),(DWORD)nvKelvinDispatchPrintVertex)
    xMOV_r_i32  (rEBX,mMEM32(global.kelvin.pContext))
    xPUSH_r     (rEDI)
    xPUSH_r     (rEBX)
    xCALL_rm    (rmREG(rEAX))
#endif

    // adjust put
    xLEA_r_rm   (rEDI,rmIND8(rEDI)) xOFS8(dwDstIndex)
}

//
// range limit <only on NT> (dummy must be eax, ebx, ecx or edx)
//
void nvKelvinILCompile_limit
(
    DWORD regLimit,
    DWORD regDummy
)
{
    assert ((regDummy == rEAX)
         || (regDummy == rEBX)
         || (regDummy == rECX)
         || (regDummy == rEDX));

#ifdef WINNT
    xXOR_r_rm   (regDummy,rmREG(regDummy))
    xCMP_r_i32  (regLimit,mMEM32(global.dwMaxVertexOffset))
    xSETAE_rm8  (rmREG(regDummy))
    xDEC_rm     (rmREG(regDummy))
    xAND_r_rm   (regLimit,rmREG(regDummy))
#endif
}



// prefetch data
//  will trash ESI, ECX, EDX, EAX
void nvKelvinILCompile_prefetch
(
    BOOL bIsIndexed,
    BOOL bLegacyStrides
)
{
    if (bIsIndexed)
    {
        // prefetch & range check indices
        DWORD labelPrefetch1;
        DWORD labelPrefetch2;
        DWORD labelPrefetch3;
        DWORD labelPrefetch4;
        xLABEL      (labelPrefetch1)
        xMOV_r_i32  (rESI,mMEM32(global.kelvin.dwPrefetchBase))
        xCMP_r_i32  (rESI,mMEM32(global.kelvin.pIndices))
        xLABEL      (labelPrefetch2)
        xJA32       (0)

        xMOV_rm_imm (rmREG(rECX),2048 / global.kelvin.dwVertexStride)
        xLABEL      (labelPrefetch4)
        {
            xPUSH_r     (rECX)
            // read index
            xXOR_r_rm   (rEDX,rmREG(rEDX))
       x16r xMOV_r_rm   (rDX,rmIND(rESI))
            xLEA_r_rm   (rESI,rmIND8(rESI)) xOFS8(2)
            if (bLegacyStrides)
            {
                xMOV_r_rm   (rEAX,rmREG(rESI))
                xSUB_r_i32  (rEAX,mMEM32(global.kelvin.pIndexBase))
                xAND_rm_imm (rmREG(rEAX),6)
                xXOR_r_rm   (rECX,rmREG(rECX))
                xCMP_rm_imm (rmREG(rEAX),6)
                xSETNZ_rm8  (rmREG(rCL))
                xDEC_rm     (rmREG(rECX))
                xAND_rm_imm (rmREG(rECX),2)
                xADD_r_rm   (rESI,rmREG(rECX))
            }
            ILCCompile_mul (rEDX,global.kelvin.dwVertexStride);
            // range limit
            nvKelvinILCompile_limit (rEDX,rECX);
            // compute vertex address
            xADD_r_i32  (rEDX,mMEM32(global.kelvin.pVertices))
            // prefetch
            xMOV_r_rm   (rEAX,rmIND(rEDX))
            if (global.kelvin.dwVertexStride > 32)
            {
                xMOV_r_rm   (rEAX,rmIND8(rEDX)) xOFS8(32)
            }
            xMOV_rm_imm (rmREG(rEAX),0)
            // end of valid run?
            xPOP_r      (rECX)
            xCMP_r_i32  (rESI,mMEM32(global.kelvin.dwPrefetchMax))
            xLABEL      (labelPrefetch3)
            xJAE32      (0)
            // next
            xDEC_rm     (rmREG(rECX))
        }
        xJNZ32      (labelPrefetch4)
        xTARGET_b32 (labelPrefetch3)
        xMOV_i32_r  (mMEM32(global.kelvin.dwPrefetchBase),rESI)
        xTARGET_b32 (labelPrefetch2)
    }
    else
    {
        // prefetch & range check indices
        DWORD labelPrefetch1;
        DWORD labelPrefetch2;
        DWORD labelPrefetch3;
        DWORD labelPrefetch4;
        xLABEL      (labelPrefetch1)
        xMOV_r_i32  (rESI,mMEM32(global.kelvin.dwPrefetchBase))
        xCMP_r_i32  (rESI,mMEM32(global.kelvin.pVertices))
        xLABEL      (labelPrefetch2)
        xJA32       (0)

        xMOV_rm_imm (rmREG(rECX),2048 / global.kelvin.dwVertexStride)
        xLABEL      (labelPrefetch4)
        {
            // prefetch
            xMOV_r_rm   (rEAX,rmIND(rESI))
            xMOV_rm_imm (rmREG(rEAX),0)
            xADD_rm_imm (rmREG(rESI),32)
            // end of valid run?
            xCMP_r_i32  (rESI,mMEM32(global.kelvin.dwPrefetchMax))
            xLABEL      (labelPrefetch3)
            xJAE32      (0)
            // next
            xDEC_rm     (rmREG(rECX))
        }
        xJNZ32      (labelPrefetch4)
        xTARGET_b32 (labelPrefetch3)
        xMOV_i32_r  (mMEM32(global.kelvin.dwPrefetchBase),rESI)
        xTARGET_b32 (labelPrefetch2)
    }
}

#endif KELVIN_ILC


#endif  // NVARCH == 0x20

