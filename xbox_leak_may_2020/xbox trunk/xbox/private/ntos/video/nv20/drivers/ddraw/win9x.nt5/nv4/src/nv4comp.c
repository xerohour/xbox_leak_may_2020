/*
 * Copyright 1993-1998 NVIDIA, Corporation.  All rights reserved.
 *
 * THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
 * NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
 * IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.
 */

/********************************* Direct 3D *******************************\
*                                                                           *
* Module: NV4COMP.C                                                         *
*   NV4 Triangle Inner Loop Compiler                                        *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Ben de Waal                 08/27/98 - Created                      *
*                                                                           *
\***************************************************************************/
#include <math.h>
#include <windows.h>
#include "nvd3ddrv.h"
#include "ddrvmem.h"
#include "nv32.h"
#include "nvddobj.h"
#include "d3dinc.h"
#include "nvd3dmac.h"
#include "nv3ddbg.h"
#include "nv4vxmac.h"

#ifdef NV_FASTLOOPS

#ifdef NV_DUMPLOOPS
#include <stdio.h>
#endif

#include "..\x86\x86.h"

/*
 * switches
 */
//#define PRINT_NAME              // force inner loop name at creation (even for retail builds)
//#define CHECK_VALID_PUSHER      // int3 when edi goes below zero (which indicates a push error)
//#define PIX1

/*
 * constants
 */
#define LUIFLAG_FOGMODE         0x00000003
#define LUIFLAG_STRIDES         0x0000000c
#define LUIFLAG_INDEXED         0x00000010
#define LUIFLAG_FVF             0x00000020
#define LUIFLAG_DX6             0x00000040
#define LUIFLAG_FIXEDWBUF       0x00000080 // ff = 256 inner loops
#ifdef NV_CONTROLTRAFFIC
#define LUIFLAG_ZFRONT          0x00000100
#define LUIFLAG_ZBACK           0x00000200 // 3ff = 1024 inner loops
#endif //NV_CONTROLTRAFFIC

#define LUIFLAG_BASIC           0x00000000
#define LUIFLAG_MMX             0x00010000
#define LUIFLAG_KATMAI          0x00020000
#define LUIFLAG_AMD             0x00040000

#define LUIMASK_BRANCH          0x0000ffff

#ifdef PIX1
#define VC_DX5_AND              7
#define VC_DX5_ADD              3
#define VC_DX6_AND              3
#define VC_DX6_ADD              1
#else
#define VC_DX5_AND              15
#define VC_DX5_ADD              7
#define VC_DX6_AND              7
#define VC_DX6_ADD              3
#endif

/*
 * types
 */
typedef (__stdcall *PFNINNERLOOP)(void);

/*
 * structures
 */

/*
 * macros
 */
#define ilcData         global.dwILCData
#define ilcCount        global.dwILCCount
#define ilcMax          global.dwILCMax

/*
 * short-lived globals
 */
DWORD  ilcFlags;

/*
 * prototypes
 */
DWORD ILCCompileCode (DWORD);

/*
 * helper debug code
 */
#ifdef DEBUG
void __stdcall nvDebugFlush (DWORD dwType)
{
    dbgFlushType(pCurrentContext,dwType);
}
void __stdcall nvDebugShowVertex (DWORD *pdwAddr)
{
    dbgDisplayVertexData (pdwAddr[1],pdwAddr[2],pdwAddr[3],pdwAddr[4],
                          pdwAddr[5],pdwAddr[6],pdwAddr[7],pdwAddr[8]);
}
#endif // DEBUG

#ifdef NV_PROFILE
DWORD nvpTriCount;
DWORD nvpTriToGo;
float nvpTriArea;

void nvProfileLog (void)
{
    if (nvpTriCount > 40)
    {
        /*
         * flush HW
         */
        NV_D3D_GLOBAL_SAVE();
        nvFlushDmaBuffers();
        NV_D3D_GLOBAL_SETUP();

        /*
         * stop stopwatch
         */
        NVP_STOP (NVP_C_PIXPERSEC);

        /*
         * log update event
         */
        nvpLogEvent (NVP_E_TRIDISP);

        /*
         * log count
         */
        if (nvpTriCount)
        {
            nvpLogCount (NVP_C_TRICOUNT,nvpTriCount);
        }

        /*
         * log average area
         */
        nvpTriArea *= 0.5f;
        if (nvpTriCount)
        {
            nvpLogCount (NVP_C_TRIAREA,(DWORD)(nvpTriArea / (float)nvpTriCount));
        }

        /*
         * log tri per second
         */
        if (nvpTriCount) {
            double a;

            a  = 1.0e6 * (double)global.dwProcessorSpeed / (double)nvpTime[NVP_C_PIXPERSEC];
            a *= (double)nvpTriArea;
            nvpLogCount (NVP_C_PIXPERSEC,(DWORD)a);
        }
    }

    /*
     * start next
     */
    nvpTriArea  = 0.0f;
    nvpTriCount = 0;
    nvpTriToGo  = nvpTriPerSecWindow;
    NVP_START (NVP_C_PIXPERSEC);
}

void __stdcall nvProfileArea (float fArea)
{
    /*
     * total area
     */
    global.fTriangleArea += fArea;

    /*
     * pix per sec
     */
    if (nvpEnableBits & (1 << NVP_C_PIXPERSEC))
    {
        /*
         * add up area
         */
        nvpTriCount ++;
        nvpTriArea  += fArea;

        /*
         * time yet
         */
        if (!(--nvpTriToGo))
        {
            nvProfileLog();
        }
    }
}
#endif //NV_PROFILE

/****************************************************************************
 * inner loop generator helpers                                             *
 ****************************************************************************/

#ifdef DEBUG
/*
 * ILCExecute_dbgFlushTriangle
 *
 * vertex data helper
 */
void __stdcall ILCExecute_dbgFlushTriangle
(
    void
)
{
    dbgFlushTriangle (pCurrentContext);
}

/*
 * ILCCompile_dbgFlushTriangle
 *
 * flushes the triangle whe the right bits are set
 */
void ILCCompile_dbgFlushTriangle
(
    void
)
{
    xMOV_i32_r  ((DWORD)&nvFifo,rESI)
    xMOV_i32_r  ((DWORD)&nvFreeCount,rEDI)
    xPUSHAD
    xMOV_rm_imm (rmREG(rEBX),ILCExecute_dbgFlushTriangle)
    xCALL_rm    (rmREG(rEBX))
    xPOPAD
    xMOV_r_i32  (rESI,(DWORD)&nvFifo)
    xMOV_r_i32  (rEDI,(DWORD)&nvFreeCount)
}

/*
 * ILCExecute_dbgDisplayVertexData
 *
 * vertex data helper
 */
void __stdcall ILCExecute_dbgDisplayVertexData
(
    DWORD* p
)
{
    dbgDisplayVertexData (p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7]);
}

/*
 * ILCCompile_dbgDisplayVertexData
 *
 * Shows vertex data when right bits are set
 */
void ILCCompile_dbgDisplayVertexData
(
    DWORD disp
)
{
    xPUSHAD
    xSUB_rm_imm8 (rmREG(rESI),disp*4 - 4)
    xPUSH_r      (rESI)
    xMOV_rm_imm  (rmREG(rEBX),ILCExecute_dbgDisplayVertexData)
    xCALL_rm     (rmREG(rEBX))
    xPOPAD
}

#endif //DEBUG

/*
 * ILCCompile_mulvertexstride
 *
 * reg = reg * fvfData.dwVertexStride
 */
__inline void ILCCompile_mulvertexstride
(
    DWORD reg
)
{
    if (ilcFlags & LUIFLAG_FVF)
    {
        xIMUL_r_i32 (reg,(DWORD)&fvfData.dwVertexStride);
    }
    else
    {
        xSHL_rm_imm (rmREG(reg),5)
    }
}

/*
 * ILCCompile_reserve
 *
 * check and wait for enough space in fifo
 */
void ILCCompile_reserve
(
    DWORD dwCount
)
{
    DWORD label1;
    DWORD label2;

#ifdef CHECK_VALID_PUSHER
    {
        DWORD l;
        xTEST_rm_imm (rmREG(rEDI),0x80000000)
        l = ilcCount;
        xJZ          (0)
        xINT3
        *(BYTE*)(ilcData + l + 1) = (BYTE)(ilcCount - (l + 2));
    }
#endif

    xCMP_rm_imm  (rmREG(rEDI),dwCount)
    label1 = ilcCount;
    xJA          (0)
    xMOV_i32_r   ((DWORD)&nvFifo,rESI)
    xMOV_i32_r   ((DWORD)&nvFreeCount,rEDI)
    xMOV_rm_imm  (rmREG(rEBX),(DWORD)nvGetDmaFreeCount)
    label2 = ilcCount;
    xPUSH_imm    (dwCount)
    xCALL_rm     (rmREG(rEBX))
    xCMP_rm_imm  (rmREG(rEAX),dwCount)
    xJL          (label2)
    xMOV_r_rm    (rEDI,rmREG(rEAX))
    xMOV_r_i32   (rESI,(DWORD)&nvFifo)

#ifdef CHECK_VALID_PUSHER
    {
        DWORD l;
        xTEST_rm_imm (rmREG(rEDI),0x80000000)
        l = ilcCount;
        xJZ          (0)
        xINT3
        *(BYTE*)(ilcData + l + 1) = (BYTE)(ilcCount - (l + 2));
    }
#endif

    *(BYTE*)(ilcData + label1 + 1) = (BYTE)(ilcCount - (label1 + 2));
}

/*
 * ILCCompile_cullcheck
 *
 * do a cull check on the triangle loaded in dwVertex0..2
 * code ends with the z flag set to indicate culled (do not draw)
 *
 * for dx5 this routine will also attempt to minimize specular
 */
__inline void ILCCompile_cullcheck
(
    void
)
{
    /*
     * specular check
     */
    if (!(ilcFlags & LUIFLAG_DX6))
    {
        DWORD label0;
        DWORD label1;
        DWORD label2;
        DWORD label3;
        DWORD label4;

        /*
         * check for specular enable
         */
        xMOV_r_i32 (rEAX,(DWORD)&global.dwSpecularState)
        xTEST_rm_r (rmREG(rEAX),rEAX)
        label0 = ilcCount;
        xJZ32      (0)

        /*
         * fetch specular into ebx,ecx and edx
         */
        xMOV_r_i32 (rEBX,(DWORD)&global.dwVertex0)
        xMOV_r_i32 (rECX,(DWORD)&global.dwVertex1)
        xMOV_r_i32 (rEDX,(DWORD)&global.dwVertex2)
        if (ilcFlags & LUIFLAG_FVF)
        {
            xMOV_r_i32 (rEAX,(DWORD)&fvfData.dwSpecularMask)
            xMOV_r_i32 (rEBP,(DWORD)&fvfData.dwSpecularOffset)

            xAND_r_rm  (rEBX,rmREG(rEAX))
                xAND_r_rm  (rECX,rmREG(rEAX))
                    xAND_r_rm  (rEDX,rmREG(rEAX))

            xADD_r_rm  (rEBX,rmREG(rEBP))
                xADD_r_rm  (rECX,rmREG(rEBP))
                    xADD_r_rm  (rEDX,rmREG(rEBP))
        }
        else
        {
            xMOV_r_rm (rEBX,rmIND8(rEBX)) xOFS8 (20) // spec
            xMOV_r_rm (rECX,rmIND8(rECX)) xOFS8 (20) // spec
            xMOV_r_rm (rEDX,rmIND8(rEDX)) xOFS8 (20) // spec
        }
        /*
         * combine and check if tri needs specular
         */
        xOR_r_rm     (rEBX,rmREG(rECX))
        xOR_r_rm     (rEBX,rmREG(rEDX))
        xTEST_rm_imm (rmREG(rEBX),0x00fcfcfc)
        label1 = ilcCount;
        xJZ32        (0)
        /*
         * check if we have to change the state
         */
        xMOV_r_i32  (rEAX,(DWORD)&global.dwSpecCurr)
        xTEST_rm_r  (rmREG(rEAX),rEAX)
        label2 = ilcCount;
        xJNZ32      (0)
        /*
         * enable
         *  eax is zero when we get here
         */
        xMOV_i32_r  ((DWORD)&global.dwSpecCount,rEAX)

        xINC_rm     (rmREG(rEAX))
        xMOV_i32_r  ((DWORD)&global.dwSpecCurr,rEAX)

        xMOV_r_i32  (rEAX,(DWORD)&global.dwBlend)
        xOR_rm_imm  (rmREG(rEAX),DRF_DEF(054,_BLEND,_SPECULARENABLE,_TRUE))

        label3 = ilcCount;
        xJMP        (0)
        /*
         * disable
         */
        *(DWORD*)(ilcData + label1 + 2) = (ilcCount - (label1 + 6));

        xMOV_r_i32   (rEAX,(DWORD)&global.dwSpecCount)
        xINC_rm      (rmREG(rEAX))
        xMOV_i32_r   ((DWORD)&global.dwSpecCount,rEAX)
        xCMP_rm_imm  (rmREG(rEAX),5)
        label4 = ilcCount;
        xJNZ32       (0)

        xXOR_r_rm   (rEAX,rmREG(rEAX))
        xMOV_i32_r  ((DWORD)&global.dwSpecCurr,rEAX)

        xMOV_r_i32  (rEAX,(DWORD)&global.dwBlend)
        xAND_rm_imm (rmREG(rEAX),0xffff0fff)

        *(DWORD*)(ilcData + label3 + 1) = ilcCount - (label3 + 5);

        xPUSH_r      (rEAX)
        ILCCompile_reserve (2);
        xMOV_rm_imm  (rmREG(rEBX),((1<<2)<<16) | (NV_DD_SPARE<<13) | NV054_BLEND)
        xPOP_r       (rEAX)
        xMOV_rm_r    (rmIND(rESI),rEBX)
        xMOV_rm_r    (rmIND8(rESI),rEAX) xOFS8(4)
        xADD_rm_imm8 (rmREG(rESI),2*4)
        xSUB_rm_imm8 (rmREG(rEDI),2)

        /*
         * done
         */
        *(DWORD*)(ilcData + label0 + 2) = (ilcCount - (label0 + 6));
        *(DWORD*)(ilcData + label2 + 2) = (ilcCount - (label2 + 6));
        *(DWORD*)(ilcData + label4 + 2) = (ilcCount - (label4 + 6));
    }

    /*
     * cull
     */
    xMOV_r_i32 (rEAX,(DWORD)&global.dwVertex0)
    xFLD_rm32  (rmIND(rEAX))
    xMOV_r_i32 (rEBX,(DWORD)&global.dwVertex1)
    xFLD_rm32  (rmIND(rEBX))
    xFXCH_st   (rST1)
    xFSUBP_st  (rST1)
    xMOV_r_i32 (rECX,(DWORD)&global.dwVertex2)
    xFLD_rm32  (rmIND8(rECX)) xOFS8(4)
    xFXCH_st   (rST1)
    xFLD_rm32  (rmIND8(rEAX)) xOFS8(4)
    xFXCH_st   (rST1)
    xFLD_rm32  (rmIND(rECX))
    xFXCH_st   (rST3)
    xFLD_rm32  (rmIND(rEAX))
    xFXCH_st   (rST3)
    xFSUBP_st  (rST1)
    xFXCH_st   (rST3)
    xFLD_rm32  (rmIND8(rEBX)) xOFS8(4)
    xFXCH_st   (rST3)
    xFSUBP_st  (rST1)
    xFXCH_st   (rST3)
    xFLD_rm32  (rmIND8(rEAX)) xOFS8(4)
    xFXCH_st   (rST2)
    xFMULP_st  (rST1)
    xFXCH_st   (rST1)
    xFSUBP_st  (rST2)
    xFXCH_st   (rST2)
    xFMULP_st  (rST1)
    xFSUBP_st  (rST1)
    xFSTP_i32  ((DWORD)&global.dwTemp)
    xMOV_r_i32 (rEAX,(DWORD)&global.dwTemp)
    xXOR_r_i32 (rEAX,(DWORD)&global.dwCullMask1)
    xTEST_r_i32(rEAX,(DWORD)&global.dwCullMask2)
}

/*
 * ILCCompile_move
 *
 * moves memory from src to dest
 *  use best algorithm depoending on architecture
 *  will thrash eax,ebp,ebx,edx,ecx and mmx/xmmx
 *
 */
void ILCCompile_move
(
    DWORD dwDst,        // NULL means to ebx
    DWORD dwSrc,        // NULL means from edx
    DWORD dwCount       // 0 means ecx has the number of bytes
)
{
    BOOL  isKNI = ilcFlags & LUIFLAG_KATMAI;
    BOOL  isMMX = ilcFlags & LUIFLAG_MMX;
    BOOL  isS16 = dwSrc && !(dwSrc & 15); // true when source is guaranteed 16 byte aligned
    BOOL  isD16 = dwDst && !(dwDst & 15); // true when dest is guaranteed 16 byte aligned
    DWORD dwMax = isKNI ? (4*16) : (isMMX ? (4*8) : (4*2)); // cannot be > 128
    DWORD label1,label2;
    BOOL  mmx = FALSE;

    /*
     * load src and dest
     */
    if (dwSrc)
    {
        xMOV_rm_imm (rmREG(rEDX),dwSrc)
    }
    if (dwDst)
    {
        xMOV_rm_imm (rmREG(rEBX),dwDst)
    }

    /*
     * constant count
     */
    if ((dwCount) && (dwCount <= dwMax))
    {
        DWORD ofs = 0;
        if (isKNI)
        {
            DWORD count = dwCount >> 4;
            DWORD index;
            for (index = 0; index < count; index++)
            {
                if (isS16) { xMOVAPS_r_rm (index,rmIND8(rEDX)) xOFS8(ofs+index*16) }
                      else { xMOVLPS_r_rm (index,rmIND8(rEDX)) xOFS8(ofs+index*16) xMOVHPS_r_rm (index,rmIND8(rEDX)) xOFS8(ofs+index*16+8) }
            }
            for (index = 0; index < count; index++)
            {
                if (isD16) { xMOVAPS_rm_r (rmIND8(rEBX),index) xOFS8(ofs+index*16) }
                      else { xMOVLPS_rm_r (rmIND8(rEBX),index) xOFS8(ofs+index*16) xMOVHPS_rm_r (rmIND8(rEBX),index) xOFS8(ofs+index*16+8) }
            }
            dwCount -= count * 16;
            ofs     += count * 16;
        }
        if (isMMX)
        {
            DWORD count = dwCount >> 3;
            DWORD index;
            for (index = 0; index < count; index++)
            {
                xMOVQ_r_rm (index,rmIND8(rEDX)) xOFS8(ofs+index*8)
            }
            for (index = 0; index < count; index++)
            {
                xMOVQ_rm_r (rmIND8(rEBX),index) xOFS8(ofs+index*8)
                mmx = 1;
            }
            dwCount -= count * 8;
            ofs     += count * 8;
        }
        {
            while (dwCount >= 8)
            {
                xMOV_r_rm (rEAX,rmIND8(rEDX)) xOFS8(ofs+0)
                xMOV_r_rm (rECX,rmIND8(rEDX)) xOFS8(ofs+4)
                xMOV_rm_r (rmIND8(rEBX),rEAX) xOFS8(ofs+0)
                xMOV_rm_r (rmIND8(rEBX),rECX) xOFS8(ofs+4)
                dwCount -= 8;
                ofs     += 8;
            }
            while (dwCount >= 4)
            {
                xMOV_r_rm (rEAX,rmIND8(rEDX)) xOFS8(ofs)
                xMOV_rm_r (rmIND8(rEBX),rEAX) xOFS8(ofs)
                dwCount -= 4;
                ofs     += 4;
            }
            while (dwCount)
            {
                xMOV_r_rm8 (rAL,rmIND8(rEDX)) xOFS8(ofs)
                xMOV_rm8_r (rmIND8(rEBX),rAL) xOFS8(ofs)
                dwCount -= 1;
                ofs     += 1;
            }
        }

        if (mmx) { xEMMS }

        return;
    }

    /*
     * variable count - or long constant
     */
    if (dwCount)
    {
        xMOV_rm_imm (rmREG(rECX),dwCount)
    }
    else
    {
        dwCount = 0x7fffffff; // force all code paths
    }

    /*
     * 64 byte chunks
     */
    if (isKNI && (dwCount >= 64))
    {
        label2 = ilcCount;
        xCMP_rm_imm  (rmREG(rECX),64)
        label1 = ilcCount;
        xJL          (0)
        if (isS16)
        {
            xMOVAPS_r_rm (rXMM0,rmIND(rEDX))
            xMOVAPS_r_rm (rXMM1,rmIND8(rEDX)) xOFS8 (0x10)
            xMOVAPS_r_rm (rXMM2,rmIND8(rEDX)) xOFS8 (0x20)
            xMOVAPS_r_rm (rXMM3,rmIND8(rEDX)) xOFS8 (0x30)
        }
        else
        {
            xMOVLPS_r_rm (rXMM0,rmIND(rEDX))
            xMOVHPS_r_rm (rXMM0,rmIND8(rEDX)) xOFS8 (0x08)
            xMOVLPS_r_rm (rXMM1,rmIND8(rEDX)) xOFS8 (0x10)
            xMOVHPS_r_rm (rXMM1,rmIND8(rEDX)) xOFS8 (0x18)
            xMOVLPS_r_rm (rXMM2,rmIND8(rEDX)) xOFS8 (0x20)
            xMOVHPS_r_rm (rXMM2,rmIND8(rEDX)) xOFS8 (0x28)
            xMOVLPS_r_rm (rXMM3,rmIND8(rEDX)) xOFS8 (0x30)
            xMOVHPS_r_rm (rXMM3,rmIND8(rEDX)) xOFS8 (0x38)
        }
        if (isD16)
        {
            xMOVAPS_rm_r (rmIND(rEBX),rXMM0)
            xMOVAPS_rm_r (rmIND8(rEBX),rXMM1) xOFS8 (0x10)
            xMOVAPS_rm_r (rmIND8(rEBX),rXMM2) xOFS8 (0x20)
            xMOVAPS_rm_r (rmIND8(rEBX),rXMM3) xOFS8 (0x30)
        }
        else
        {
            xMOVLPS_rm_r (rmIND(rEBX),rXMM0)
            xMOVHPS_rm_r (rmIND8(rEBX),rXMM0) xOFS8 (0x08)
            xMOVLPS_rm_r (rmIND8(rEBX),rXMM1) xOFS8 (0x10)
            xMOVHPS_rm_r (rmIND8(rEBX),rXMM1) xOFS8 (0x18)
            xMOVLPS_rm_r (rmIND8(rEBX),rXMM2) xOFS8 (0x20)
            xMOVHPS_rm_r (rmIND8(rEBX),rXMM2) xOFS8 (0x28)
            xMOVLPS_rm_r (rmIND8(rEBX),rXMM3) xOFS8 (0x30)
            xMOVHPS_rm_r (rmIND8(rEBX),rXMM3) xOFS8 (0x38)
        }
        xADD_rm_imm8 (rmREG(rEBX),64)
        xADD_rm_imm8 (rmREG(rEDX),64)
        xSUB_rm_imm8 (rmREG(rECX),64)
        xJNZ         (label2)
        *(BYTE*)(ilcData + label1 + 1) = (BYTE)(ilcCount - (label1 + 2));

        dwCount &= ~63;
    }

    /*
     * 16-byte chunks
     */
    if (dwCount >= 16) {
        label2 = ilcCount;
        xCMP_rm_imm  (rmREG(rECX),16)
        label1 = ilcCount;
        xJL          (0)
        if (isKNI)
        {
            if (isS16) { xMOVAPS_r_rm (rXMM0,rmIND(rEDX)) }
                  else { xMOVLPS_r_rm (rXMM0,rmIND(rEDX)) xMOVHPS_r_rm (rXMM0,rmIND8(rEDX)) xOFS8 (8) }
            if (isD16) { xMOVAPS_rm_r (rmIND(rEBX),rXMM0) }
                  else { xMOVLPS_rm_r (rmIND(rEBX),rXMM0) xMOVHPS_rm_r (rmIND8(rEBX),rXMM0) xOFS8 (8) }
        } else if (isMMX)
        {
            xMOVQ_r_rm (rMM0,rmIND(rEDX))
            xMOVQ_r_rm (rMM1,rmIND8(rEDX)) xOFS8(8)
            xMOVQ_rm_r (rmIND(rEBX),rMM0)
            xMOVQ_rm_r (rmIND8(rEBX),rMM1) xOFS8(8)
            mmx = 1;
        } else
        {
            xMOV_r_rm (rEAX,rmIND(rEDX))
            xMOV_r_rm (rEBP,rmIND8(rEDX)) xOFS8(4)
            xMOV_rm_r (rmIND(rEBX),rEAX)
            xMOV_rm_r (rmIND8(rEBX),rEBP) xOFS8(4)
            xMOV_r_rm (rEAX,rmIND8(rEDX)) xOFS8(8)
            xMOV_r_rm (rEBP,rmIND8(rEDX)) xOFS8(12)
            xMOV_rm_r (rmIND8(rEBX),rEAX) xOFS8(8)
            xMOV_rm_r (rmIND8(rEBX),rEBP) xOFS8(12)
        }
        xADD_rm_imm8 (rmREG(rEBX),16)
        xADD_rm_imm8 (rmREG(rEDX),16)
        xSUB_rm_imm8 (rmREG(rECX),16)
        xJNZ         (label2)
        *(BYTE*)(ilcData + label1 + 1) = (BYTE)(ilcCount - (label1 + 2));

        dwCount &= ~15;
    }

    /*
     * 4-bytes
     */
    if (dwCount >= 4) {
        label2 = ilcCount;
        xCMP_rm_imm  (rmREG(rECX),4)
        label1 = ilcCount;
        xJL          (0)
        xMOV_r_rm (rEAX,rmIND(rEDX))
        xADD_rm_imm8 (rmREG(rEDX),4)
        xMOV_rm_r (rmIND(rEBX),rEAX)
        xADD_rm_imm8 (rmREG(rEBX),4)
        xSUB_rm_imm8 (rmREG(rECX),4)
        xJNZ         (label2)
        *(BYTE*)(ilcData + label1 + 1) = (BYTE)(ilcCount - (label1 + 2));

        dwCount &= ~3;
    }

    /*
     * 1-bytes
     */
    if (dwCount) {
        xTEST_rm_r   (rmREG(rECX),rECX)
        label1 = ilcCount;
        xJZ          (0)
        label2 = ilcCount;
        xMOV_r_rm8   (rAL,rmIND(rEDX))
        xINC_rm      (rmREG(rEDX))
        xMOV_rm8_r   (rmIND(rEBX),rAL)
        xINC_rm      (rmREG(rEBX))
        xDEC_rm      (rmREG(rECX))
        xJNZ         (label2)
        *(BYTE*)(ilcData + label1 + 1) = (BYTE)(ilcCount - (label1 + 2));
    }

    if (mmx) { xEMMS }
}

/*
 * ILCCompile_vertex
 *
 * send vertex to hardware
 *  edx = vertex start addr
 *  ebx = hw vertex position
 */
void ILCCompile_vertex
(
    DWORD dwPosition //= 0xffffffff for register ebx
)
{
    DWORD dwHWSize = (ilcFlags & LUIFLAG_DX6) ? 11 : 9;

    /*
     * select vertex type
     */
    if (dwPosition != 0xffffffff)
    {
        xMOV_rm_imm (rmREG(rEBX),((ilcFlags & LUIFLAG_DX6)
                                 ?(0x0028e400 | dwPosition*40)
                                 :(0x0020e400 | dwPosition*32)))
    }
    else
    {
        if (ilcFlags & LUIFLAG_DX6)
        {
            xLEA_r_rm   (rEBX,rmSIB) xSIB(rEBX,rEBX,x4)
            xSHL_rm_imm (rmREG(rEBX),3)
            xOR_rm_imm  (rmREG(rEBX),0x0028e400) // dx6 class specific
        }
        else
        {
            xSHL_rm_imm (rmREG(rEBX),5)
            xOR_rm_imm  (rmREG(rEBX),0x0020e400) // dx5 class specific
        }
    }

    /*
     * individual vertex overrides
     *  non-pipelined code for special cases (i.e need no gathering)
     *  ebx still has vertex type
     */
#ifdef NV_CONTROLTRAFFIC
    if (!(ilcFlags & (LUIFLAG_DX6 | LUIFLAG_FOGMODE | LUIFLAG_FVF | LUIFLAG_FIXEDWBUF | LUIFLAG_ZFRONT | LUIFLAG_ZBACK)))
#else
    if (!(ilcFlags & (LUIFLAG_DX6 | LUIFLAG_FOGMODE | LUIFLAG_FVF | LUIFLAG_FIXEDWBUF)))
#endif
    {
        /*
         * DX5 TL vertex
         */
        if (ilcFlags & LUIFLAG_KATMAI) // katmai version
        {
            xMOV_rm_r  (rmIND(rESI),rEBX)
            xMOVLPS_r_rm (rXMM0,rmIND(rEDX))
            xMOVHPS_r_rm (rXMM0,rmIND8(rEDX)) xOFS8(8)
            xMOVLPS_r_rm (rXMM1,rmIND8(rEDX)) xOFS8(16)
            xMOVHPS_r_rm (rXMM1,rmIND8(rEDX)) xOFS8(24)
            xMOVLPS_rm_r (rmIND8(rESI),rXMM0) xOFS8(4)
            xMOVHPS_rm_r (rmIND8(rESI),rXMM0) xOFS8(12)
            xMOVLPS_rm_r (rmIND8(rESI),rXMM1) xOFS8(20)
            xMOVHPS_rm_r (rmIND8(rESI),rXMM1) xOFS8(28)
            xADD_rm_imm8 (rmREG(rESI),9*4)
            xSUB_rm_imm8 (rmREG(rEDI),9)
        }
        else if (ilcFlags & LUIFLAG_MMX) // mmx version
        {
                xMOV_rm_r  (rmIND(rESI),rEBX)
            xMOVQ_r_rm (rMM0,rmIND(rEDX))
            xMOVQ_r_rm (rMM1,rmIND8(rEDX)) xOFS8(8)
            xMOVQ_r_rm (rMM2,rmIND8(rEDX)) xOFS8(16)
            xMOVQ_r_rm (rMM3,rmIND8(rEDX)) xOFS8(24)
                xMOVQ_rm_r (rmIND8(rESI),rMM0) xOFS8(4)
                xMOVQ_rm_r (rmIND8(rESI),rMM1) xOFS8(12)
                xMOVQ_rm_r (rmIND8(rESI),rMM2) xOFS8(20)
                xMOVQ_rm_r (rmIND8(rESI),rMM3) xOFS8(28)
            xEMMS
            xADD_rm_imm8   (rmREG(rESI),9*4)
            xSUB_rm_imm8   (rmREG(rEDI),9)
        }
        else // basic version
        {
                xMOV_rm_r  (rmIND(rESI),rEBX)
            xMOV_r_rm  (rEAX,rmIND(rEDX))
            xMOV_r_rm  (rEBX,rmIND8(rEDX)) xOFS8(4)
            xMOV_r_rm  (rECX,rmIND8(rEDX)) xOFS8(8)
            xMOV_r_rm  (rEBP,rmIND8(rEDX)) xOFS8(12)
                xMOV_rm_r  (rmIND8(rESI),rEAX) xOFS8(4)
                xMOV_rm_r  (rmIND8(rESI),rEBX) xOFS8(8)
                xMOV_rm_r  (rmIND8(rESI),rECX) xOFS8(12)
                xMOV_rm_r  (rmIND8(rESI),rEBP) xOFS8(16)
            xMOV_r_rm  (rEAX,rmIND8(rEDX)) xOFS8(16)
            xMOV_r_rm  (rEBX,rmIND8(rEDX)) xOFS8(20)
            xMOV_r_rm  (rECX,rmIND8(rEDX)) xOFS8(24)
            xMOV_r_rm  (rEBP,rmIND8(rEDX)) xOFS8(28)
                xMOV_rm_r  (rmIND8(rESI),rEAX) xOFS8(20)
                xMOV_rm_r  (rmIND8(rESI),rEBX) xOFS8(24)
                xMOV_rm_r  (rmIND8(rESI),rECX) xOFS8(28)
                xMOV_rm_r  (rmIND8(rESI),rEBP) xOFS8(32)
            xADD_rm_imm8   (rmREG(rESI),9*4)
            xSUB_rm_imm8   (rmREG(rEDI),9)
        }

#ifdef CHECK_VALID_PUSHER
    {
        DWORD l;
        xTEST_rm_imm (rmREG(rEDI),0x80000000)
        l = ilcCount;
        xJZ          (0)
        xINT3
        *(BYTE*)(ilcData + l + 1) = (BYTE)(ilcCount - (l + 2));
    }
#endif

#ifdef DEBUG
        /*
         * debug code
         */
        ILCCompile_dbgDisplayVertexData(9);
#endif // DEBUG

        /*
         * done
         */
        return;
    }


    /*
     * gather stage
     *  mostly does fvf decode and/or prepare for fog
     *  ebx still has vertex type
     *
     * gather buffer layout - (dx5 ? 36 : 44) bytes total
     *  +0  +4  +8  +12 +16 +20 +24 +28 +32 +36 +40
     *  vtx sx  sy  sz  rhw col spc u0  v0  u1  v1
     */
    if (ilcFlags & LUIFLAG_FVF)
    {
            xMOV_i32_r ( 0+(DWORD)&global.dwVertexGatherBuf,rEBX);
        // x,y,z
        xMOV_r_rm  (rEAX,rmIND(rEDX))
        xMOV_r_rm  (rEBX,rmIND8(rEDX)) xOFS8(4)
        xMOV_r_rm  (rECX,rmIND8(rEDX)) xOFS8(8)
            xMOV_i32_r ( 4+(DWORD)&global.dwVertexGatherBuf,rEAX);
            xMOV_i32_r ( 8+(DWORD)&global.dwVertexGatherBuf,rEBX);
            xMOV_i32_r (12+(DWORD)&global.dwVertexGatherBuf,rECX);
        // color,specular,u0,v0
        xMOV_r_rm  (rEAX,rmREG(rEDX))
            xMOV_r_rm  (rEBX,rmREG(rEDX))
                xMOV_r_rm  (rECX,rmREG(rEDX))
                    xMOV_r_rm  (rEBP,rmREG(rEDX))
        xAND_r_i32 (rEAX,(DWORD)&fvfData.dwRHWMask)
            xAND_r_i32 (rEBX,(DWORD)&fvfData.dwARGBMask)
                xAND_r_i32 (rECX,(DWORD)&fvfData.dwSpecularMask)
                    xAND_r_i32 (rEBP,(DWORD)&fvfData.dwUVMask)
        xADD_r_i32 (rEAX,(DWORD)&fvfData.dwRHWOffset)
            xADD_r_i32 (rEBX,(DWORD)&fvfData.dwARGBOffset)
                xADD_r_i32 (rECX,(DWORD)&fvfData.dwSpecularOffset)
                    xADD_r_i32 (rEBP,(DWORD)&fvfData.dwUVOffset)
        xMOV_r_rm (rEAX,rmIND(rEAX))
            xMOV_r_rm (rEBX,rmIND(rEBX))
                xMOV_r_rm (rECX,rmIND(rECX))
                    xMOV_r_i32 (rEDX,(DWORD)&global.pdwUVOffset)
        xMOV_i32_r (16+(DWORD)&global.dwVertexGatherBuf,rEAX);
                    xMOV_r_rm  (rEAX,rmIND(rEDX))
            xMOV_i32_r (20+(DWORD)&global.dwVertexGatherBuf,rEBX);
                    xDS xMOV_r_rm (rEBX,rmSIB) xSIB(rEAX,rEBP,x1)
                xMOV_i32_r (24+(DWORD)&global.dwVertexGatherBuf,rECX);
                        xDS xMOV_r_rm (rECX,rmSIB8) xSIB(rEAX,rEBP,x1) xOFS8 (4)
                    xMOV_i32_r (28+(DWORD)&global.dwVertexGatherBuf,rEBX);
                        xMOV_i32_r (32+(DWORD)&global.dwVertexGatherBuf,rECX);
        // u1,v1
        if (ilcFlags & LUIFLAG_DX6)
        {
                    xMOV_r_rm  (rEAX,rmIND8(rEDX)) xOFS8(4)
                    xDS xMOV_r_rm (rEBX,rmSIB) xSIB(rEAX,rEBP,x1)
                        xDS xMOV_r_rm (rECX,rmSIB8) xSIB(rEAX,rEBP,x1) xOFS8 (4)
                    xMOV_i32_r (36+(DWORD)&global.dwVertexGatherBuf,rEBX);
                        xMOV_i32_r (40+(DWORD)&global.dwVertexGatherBuf,rECX);
        }
    }
    else
    {
        xMOV_i32_r (0+(DWORD)&global.dwVertexGatherBuf,rEBX);
        ILCCompile_move (4+(DWORD)&global.dwVertexGatherBuf,0/*edx*/,32); // always TL when not FVF
    }

    /*
     * apply fog
     */
    if (ilcFlags & LUIFLAG_FOGMODE)
    {
        BOOL isExp2 = (ilcFlags & LUIFLAG_FOGMODE) == 2;
        BOOL isLin  = (ilcFlags & LUIFLAG_FOGMODE) == 3;

        /*
         * st(0) = (global.dvWNear == 1.0f) ? z : approx(1.0 / rhw)
         */
        xMOV_rm_imm  (rmREG(rEBX),0x3f800000*2)
        xSUB_r_i32   (rEBX,16+(DWORD)&global.dwVertexGatherBuf) // rhw
        xMOV_i32_r   ((DWORD)&global.dwTemp,rEBX)
        xFLD_i32     ((DWORD)&global.dwTemp)
        xFLD_st      (rST0)
        xFMUL_i32    (16+(DWORD)&global.dwVertexGatherBuf) // rhw
        xFLD_i32     ((DWORD)&global.fFogC1)
        xFSUBRP_st   (rST1)
        xFMULP_st    (rST1)

        xFSTP_i32    ((DWORD)&global.dwTemp)
        xMOV_r_i32   (rEDX,12+(DWORD)&global.dwVertexGatherBuf) // z
        xAND_r_i32   (rEDX,(DWORD)&global.dwWMask1)

        xMOV_r_i32   (rEBX,(DWORD)&global.dwTemp)
        xAND_r_i32   (rEBX,(DWORD)&global.dwWMask2)

        xOR_r_rm     (rEBX,rmREG(rEDX))
        xMOV_i32_r   ((DWORD)&global.dwTemp,rEBX)

        xFLD_i32     ((DWORD)&global.dwTemp)

        if (isLin)
        {
            /*
             * st(0) = (fFogTableEnd - st(0)) * fFogTableLinearScale
             */
            xFLD_i32   ((DWORD)&global.fFogTableEnd)
            xFSUBRP_st (rST1)
            xFMUL_i32  ((DWORD)&global.fFogTableLinearScale)
        }
        else
        {
            /*
             * st(0) *= fFogTableDensity
             */
            xFMUL_i32  ((DWORD)&global.fFogTableDensity)

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
            xFMUL_i32   ((DWORD)&global.fFogC2)
            xFISTP_i32  ((DWORD)&global.dwTemp)
            xMOV_rm_imm (rmREG(rEAX),0x3f800000)
            xADD_i32_r  ((DWORD)&global.dwTemp,rEAX)
            xFLD_i32    ((DWORD)&global.dwTemp)
        }

        /*
         * eax = int8bits ( st(0) ) << 24
         */
        xFLD1
        xFADDP_st    (rST1)
        xFSTP_i32    ((DWORD)&global.dwTemp)
        xMOV_r_i32   (rEAX,(DWORD)&global.dwTemp)

        xCMP_rm_imm  (rmREG(rEAX),0x3f800000)
        xSETG_rm8    (rmREG(rBL))
        xSHR_rm_imm  (rmREG(rEBX),1)
        xSBB_r_rm    (rEBP,rmREG(rEBP))

        xCMP_rm_imm  (rmREG(rEAX),0x40000000)
        xSETGE_rm8   (rmREG(rBL))
        xSHR_rm_imm  (rmREG(rEBX),1)
        xSBB_r_rm    (rEBX,rmREG(rEBX))

        xSHL_rm_imm  (rmREG(rEAX),9)
        xOR_r_rm     (rEAX,rmREG(rEBX))
        xAND_r_rm    (rEAX,rmREG(rEBP))
        xAND_rm_imm  (rmREG(rEAX),0xff000000)

        /*
         * update specular
         */
        xMOV_r_i32   (rEBX,24+(DWORD)&global.dwVertexGatherBuf) // spec
        xAND_rm_imm  (rmREG(rEBX),0x00ffffff)
        xOR_r_rm     (rEBX,rmREG(rEAX))
        xMOV_i32_r   (24+(DWORD)&global.dwVertexGatherBuf,rEBX) // spec
    }

    /*
     * apply fixed w-buffer scale
     */
    if (ilcFlags & LUIFLAG_FIXEDWBUF)
    {
        xFLD_i32  (16+(DWORD)&global.dwVertexGatherBuf) // rhw
        xFMUL_i32 ((DWORD)&global.fRWFar)
        xFSTP_i32 (16+(DWORD)&global.dwVertexGatherBuf) // rhw
    }

    /*
     * optimized z
     */
#ifdef NV_CONTROLTRAFFIC
    if (ilcFlags & (LUIFLAG_ZFRONT | LUIFLAG_ZBACK))
    {
        xMOV_r_i32   (rEAX,12+(DWORD)&global.dwVertexGatherBuf) // z

        if (ilcFlags & LUIFLAG_ZFRONT)
        {
            /*
             * clamp z to 1.0f
             */
            xCMP_rm_imm  (rmREG(rEAX),0x3f800000)
            xSETG_rm8    (rmREG(rBL))
            xSHR_rm_imm  (rmREG(rEBX),1)
            xSBB_r_rm    (rEBX,rmREG(rEBX))     // ebx = (eax > 1.0f) ? -1 : 0

            xMOV_r_rm    (rECX,rmREG(rEBX))
            xNOT_rm      (rmREG(rECX))          // ecx = (eax > 1.0f) ? 0 : -1

            xAND_rm_imm  (rmREG(rEBX),0x3f800000)
            xAND_r_rm    (rEAX,rmREG(rECX))
            xOR_r_rm     (rEAX,rmREG(rEBX))
        }

        if (ilcFlags & LUIFLAG_ZBACK)
        {
            /*
             * clamp z to 0.0f
             */
            xMOV_r_rm    (rEBX,rmREG(rEAX))
            xSHL_rm_imm  (rmREG(rEBX),1)
            xSBB_r_rm    (rEBX,rmREG(rEBX))     // ebx = (eax <= -0.0f) ? -1 : 0
            xNOT_rm      (rmREG(rEBX))          // ebx = (eax <= -0.0f) ? 0 : -1
            xAND_r_rm    (rEAX,rmREG(rEBX))
        }

        xMOV_i32_r   ((DWORD)&global.dwTemp,rEAX)

        /*
         * optimize
         */
        xFLD_i32  ((DWORD)&global.dwTemp)
        xFMUL_i32 ((DWORD)&global.fCTC1);
        if (ilcFlags & LUIFLAG_ZBACK)
        {
            xFLD1
            xFSUBRP_st (rST1);
        }
        xFSTP_i32  (12+(DWORD)&global.dwVertexGatherBuf) // z
    }
#endif //NV_CONTROLTRAFFIC

    /*
     * copy to push buffer
     */
    xMOV_r_rm (rEBX,rmREG(rESI))
    ILCCompile_move (0/*ebx*/,(DWORD)&global.dwVertexGatherBuf,dwHWSize*4);
    xADD_rm_imm8   (rmREG(rESI),dwHWSize*4)
    xSUB_rm_imm8   (rmREG(rEDI),dwHWSize)

#ifdef CHECK_VALID_PUSHER
    {
        DWORD l;
        xTEST_rm_imm (rmREG(rEDI),0x80000000)
        l = ilcCount;
        xJZ          (0)
        xINT3
        *(BYTE*)(ilcData + l + 1) = (BYTE)(ilcCount - (l + 2));
    }
#endif

#ifdef DEBUG
    /*
     * debug code
     */
    ILCCompile_dbgDisplayVertexData (dwHWSize);
#endif // DEBUG

}

/*
 * ILCCompile_launch
 *
 * send vertex to hardware
 *  eax,ebx,ecx will be used if dwCode == -1
 *  eax         will be used if dwCode == -2
 */
void ILCCompile_launch
(
    DWORD dwCode // == -1 for launch out of eax,ebx and ecx, -2 for launch from packed eax
)
{
#ifdef PIX1
    dwCode = (ilcFlags & LUIFLAG_DX6) ? 0x765 : 0xfed;
#endif //PIX1

    xMOV_rm_imm  (rmREG(rEDX),((ilcFlags & LUIFLAG_DX6) ? 0x0004e540 : 0x0004e600))
    switch (dwCode)
    {
        case 0xffffffff:
        {
            xSHL_rm_imm (rmREG(rECX),8)
            xSHL_rm_imm (rmREG(rEBX),4)
            xOR_r_rm    (rEAX,rmREG(rECX))
            xOR_r_rm    (rEAX,rmREG(rEBX))
            break;
        }
        case 0xfffffffe:
        {
            break;
        }
        default:
        {
            xMOV_rm_imm (rmREG(rEAX),dwCode)
            break;
        }
    }
    xMOV_rm_r    (rmIND(rESI),rEDX)
    xMOV_rm_r    (rmIND8(rESI),rEAX) xOFS8(4)
    xADD_rm_imm8 (rmREG(rESI),2*4)
    xSUB_rm_imm8 (rmREG(rEDI),2)
}

/****************************************************************************
 * inner loop generator                                                     *
 ****************************************************************************/

/*
 * ILCCompileCode
 *
 * Returns the entry point of a loop that will emit triangles in the format
 * specified. fvfData is implicitly passed
 */
DWORD ILCCompileCode
(
    DWORD dwFlags
)
{
    DWORD label_entry;
    DWORD label_loop;
    DWORD label_warmup;
    DWORD label_draw;
    DWORD label_cullbranch;
    DWORD label_arb11;
    DWORD label_arb12;
    DWORD label_arb21;
    DWORD label_arb22;
    DWORD ilcVertexSize;

    /*
     * setup
     */
    ilcFlags      = dwFlags;
    ilcVertexSize = (ilcFlags & LUIFLAG_DX6) ? 11 : 9;

    /*
     * begin code generation
     */
    while (ilcCount & 31) { xINT3 } // branch prediction helper


    /*
     * pocket of subroutines
     * ---------------------
     */

    /*
     * arbitration code (indexed lists)
     */
    label_arb11 = ilcCount;
    xADD_rm_imm8 (rmREG(rEBX),((ilcFlags & LUIFLAG_DX6) ? VC_DX6_ADD : VC_DX5_ADD))
    xAND_rm_imm8 (rmREG(rEBX),((ilcFlags & LUIFLAG_DX6) ? VC_DX6_AND : VC_DX5_AND))
    //xCMP_r_i32   (rEBX,(DWORD)&global.dwIndex0)
    //xJZ          (label_arb11)
    label_arb12 = ilcCount;
    xJMP         (0)
    while (ilcCount & 31) { xINT3 } // branch prediction helper

    label_arb21 = ilcCount;
    xADD_rm_imm8 (rmREG(rEBX),((ilcFlags & LUIFLAG_DX6) ? VC_DX6_ADD : VC_DX5_ADD))
    xAND_rm_imm8 (rmREG(rEBX),((ilcFlags & LUIFLAG_DX6) ? VC_DX6_AND : VC_DX5_AND))
    xCMP_r_i32   (rEBX,(DWORD)&global.dwIndex0)
    xJZ          (label_arb21)
    xCMP_r_i32   (rEBX,(DWORD)&global.dwIndex1)
    xJZ          (label_arb21)
    label_arb22 = ilcCount;
    xJMP         (0)
    while (ilcCount & 31) { xINT3 } // branch prediction helper


    /*
     * function entry code
     * -------------------
     */
    label_entry = ilcCount;
    xPUSH_r     (rEBP)
    xPUSH_r     (rESI)
    xPUSH_r     (rEDI)
    xMOV_r_i32  (rESI,(DWORD)&nvFifo);
    xMOV_r_i32  (rEDI,(DWORD)&nvFreeCount);


/*
    if (ilcFlags == 0x10010)
    {
        xINT3
        //__asm int 3
    }
//*/

    /*
     * warm cache
     */
    xMOV_r_i32 (rEAX,(DWORD)&global.dwVertex0)
    xMOV_r_i32 (rEAX,(DWORD)&global.dwTemp)
    xMOV_r_i32 (rEAX,(DWORD)&global.dwVertexCache)
    if ((ilcFlags & LUIFLAG_FOGMODE) || (ilcFlags & LUIFLAG_FVF))
    {
        // gather buffer
        xMOV_r_i32 (rEAX,(DWORD)&global.dwVertexGatherBuf)
        xMOV_r_i32 (rEAX,(DWORD)&global.dwVertexGatherBuf2)
    }
    xMOV_r_rm (rEAX,rmREG(rEBP)) // kill eax so we wont stall

    /*
     * setup loop
     * ----------
     */
    if (ilcFlags & LUIFLAG_INDEXED)
    {
        /*
         * setup indices
         */
        xXOR_r_rm   (rEAX,rmREG(rEAX))
        xMOV_i32_r  ((DWORD)&global.dwIndex0,rEAX)
        xINC_r      (rEAX)
        xMOV_i32_r  ((DWORD)&global.dwIndex1,rEAX)
        xINC_r      (rEAX)
        xMOV_i32_r  ((DWORD)&global.dwIndex2,rEAX)
        switch (ilcFlags & LUIFLAG_STRIDES)
        {
            case 0x04: // strip
            case 0x08: // fan
            {
                /*
                 * cull state
                 */
                xMOV_rm_imm (rmREG(rEBX),0x210)
                xMOV_i32_r  ((DWORD)&global.dwStripFanData,rEBX)

                /*
                 * basic translation
                 */
                xMOV_r_i32   (rEAX,(DWORD)&global.pIndices)
                xMOV_r_i32   (rEDX,(DWORD)&global.pVertices)
                xMOV_r_rm    (rEBX,rmIND(rEAX))
                        xMOV_r_rm    (rECX,rmREG(rEBX))
                xADD_rm_imm8 (rmREG(rEAX),4)
                    xAND_rm_imm  (rmREG(rEBX),0xffff)
                        xSHR_rm_imm  (rmREG(rECX),16)
                xMOV_i32_r   ((DWORD)&global.pIndices,rEAX)
                    ILCCompile_mulvertexstride (rEBX);
                        ILCCompile_mulvertexstride (rECX);
                    xADD_r_rm    (rEBX,rmREG(rEDX))
                        xADD_r_rm    (rECX,rmREG(rEDX))
                    xMOV_i32_r   ((DWORD)&global.dwVertex0,rEBX)
                        xMOV_i32_r   ((DWORD)&global.dwVertex1,rECX)
                break;
            }
        }

        /*
         * vertex cache serial number update
         */
        xMOV_r_i32 (rEAX,(DWORD)&global.dwVertexSN)
        xADD_rm_imm(rmREG(rEAX),0x10000)
        xMOV_i32_r ((DWORD)&global.dwVertexSN,rEAX)
    }
    else
    {
        xMOV_r_i32  (rEAX,(DWORD)&global.pVertices)
        xMOV_i32_r ((DWORD)&global.dwVertex0,rEAX)
        xADD_r_i32  (rEAX,(DWORD)&fvfData.dwVertexStride)
        xMOV_i32_r ((DWORD)&global.dwVertex1,rEAX)
        xADD_r_i32  (rEAX,(DWORD)&fvfData.dwVertexStride)
        xMOV_i32_r ((DWORD)&global.dwVertex2,rEAX)

        switch (ilcFlags & LUIFLAG_STRIDES)
        {
            case 0x04: // strip
            case 0x08: // fan
            {
                xMOV_i32_r  ((DWORD)&global.pVertices,rEAX)
                /*
                 * cull state
                 */
                xMOV_rm_imm (rmREG(rEBX),0x210)
                xMOV_i32_r  ((DWORD)&global.dwStripFanData,rEBX)
                break;
            }
        }
    }

    /*
     * inner loop setup
     * ----------------
     */
    label_loop = ilcCount;

    /*
     * cache warm-up
     * -------------
     *
     * pIndices must be kept up to date for the warm-up phase. So does pVertices for
     *  ordered primitives
     */

    /*
     * leave the number of primitives in ebx, number of verts in dwVertSubCount
     */
    switch (ilcFlags & LUIFLAG_STRIDES)
    {
        case 0x00: // list
        case 0x0c: // legacy
        {
            // prim = 3*vert, 32 triangles per batch (96 verts)
            xMOV_r_i32  (rEAX,(DWORD)&global.dwPrimCount)
            xMOV_r_rm   (rEBX,rmREG(rEAX))
            xSUB_rm_imm (rmREG(rEBX),32)
            xSBB_r_rm   (rECX,rmREG(rECX))
            xMOV_r_rm   (rEBX,rmREG(rECX))
            xNOT_rm     (rmREG(rEBX))
            xAND_rm_imm (rmREG(rEBX),32)
            xAND_r_rm   (rECX,rmREG(rEAX))
            xOR_r_rm    (rEBX,rmREG(rECX))
            xMOV_i32_r  ((DWORD)&global.dwPrimSubCount,rEBX)
            xSUB_i32_r  ((DWORD)&global.dwPrimCount,rEBX);
            xLEA_r_rm   (rEBX,rmSIB); xSIB(rEBX,rEBX,x2);
            // result in ebx
            break;
        }
        case 0x04: // strip
        case 0x08: // fan
        {
            // prim = vert, 96 triangles per batch
            xMOV_r_i32  (rEAX,(DWORD)&global.dwPrimCount)
            xMOV_r_rm   (rEBX,rmREG(rEAX))
            xSUB_rm_imm (rmREG(rEBX),96)
            xSBB_r_rm   (rECX,rmREG(rECX))
            xMOV_r_rm   (rEBX,rmREG(rECX))
            xNOT_rm     (rmREG(rEBX))
            xAND_rm_imm (rmREG(rEBX),96)
            xAND_r_rm   (rECX,rmREG(rEAX))
            xOR_r_rm    (rEBX,rmREG(rECX))
            xMOV_i32_r  ((DWORD)&global.dwPrimSubCount,rEBX)
            xSUB_i32_r  ((DWORD)&global.dwPrimCount,rEBX);
            // result in ebx
            break;
        }
    }

    /*
     * step through ebx vertices and just fetch them
     */
    xMOV_r_rm (rECX,rmREG(rEBX));
    if (ilcFlags & LUIFLAG_INDEXED)
    {
        xMOV_r_i32 (rEBX,(DWORD)&global.pIndices)
        if ((ilcFlags & LUIFLAG_STRIDES) == 0x0c)
        {
            xMOV_rm_imm (rmREG(rEBP),3);
        }
        label_warmup = ilcCount;

        xXOR_r_rm    (rEAX,rmREG(rEAX));
   x16r xMOV_r_rm    (rAX,rmIND(rEBX));
        xADD_rm_imm8 (rmREG(rEBX),2);
        ILCCompile_mulvertexstride (rEAX);
        xADD_r_i32 (rEAX,(DWORD)&global.pVertices)
        xMOV_r_rm  (rEAX,rmIND(rEAX))
        if ((ilcFlags & LUIFLAG_STRIDES) == 0x0c)
        {
            xMOV_rm_imm (rmREG(rEAX),0) // explicit move so we don't wait for eax to do xor eax,eax
            xDEC_rm     (rmREG(rEBP))
            xSETZ_rm8   (rmREG(rAL))
            xLEA_r_rm   (rEAX,rmSIB); xSIB(rEAX,rEAX,x2);
            xOR_r_rm    (rEBP,rmREG(rEAX))
            xAND_rm_imm8(rmREG(rEAX),2)
            xADD_r_rm   (rEBX,rmREG(rEAX))
        }
        else
        {
            xMOV_r_rm   (rEAX,rmREG(rEBP)) // kill eax so uproc do not try to wait for it to come in
        }
    }
    else
    {
        xMOV_r_i32   (rEBX,(DWORD)&global.pVertices)
        xMOV_r_i32   (rEDX,(DWORD)&fvfData.dwVertexStride)
        label_warmup = ilcCount;

        xMOV_r_rm (rEAX,rmIND(rEBX))
        xMOV_r_rm (rEAX,rmREG(rEBP)) // kill eax so uproc do not try to wait for it to come in
        xADD_r_rm (rEBX,rmREG(rEDX))
    }

    xDEC_r     (rECX)
    xJNZ       (label_warmup)

    /*
     * dispatch triangles
     * ------------------
     */
    label_draw = ilcCount;

    /*
     * we have a couple of different inner loop styles
     * each are treated special (don't say I don't work for my money... although anybody
     * that has to debug this would probably want to shoot me!  ;) )
     *
     */
    switch (ilcFlags & LUIFLAG_STRIDES)
    {
        case 0x00: // list
        case 0x0c: // legacy
        {
            /*
             * translate indices to vertices
             */
            if (ilcFlags & LUIFLAG_INDEXED)
            {
                xMOV_r_i32 (rECX,(DWORD)&global.pIndices)
                xMOV_r_i32 (rEDX,(DWORD)&global.pVertices)

                xXOR_r_rm  (rEBX,rmREG(rEBX))
           x16r xMOV_r_rm  (rBX,rmIND(rECX))
                xADD_rm_imm8(rmREG(rECX),2)
                xMOV_i32_r ((DWORD)&global.dwIndex0,rEBX)
                ILCCompile_mulvertexstride (rEBX);
                xADD_r_rm  (rEBX,rmREG(rEDX))
                xMOV_i32_r ((DWORD)&global.dwVertex0,rEBX)

                xXOR_r_rm  (rEBX,rmREG(rEBX))
           x16r xMOV_r_rm  (rBX,rmIND(rECX))
                xADD_rm_imm8(rmREG(rECX),2)
                xMOV_i32_r ((DWORD)&global.dwIndex1,rEBX)
                ILCCompile_mulvertexstride (rEBX);
                xADD_r_rm  (rEBX,rmREG(rEDX))
                xMOV_i32_r ((DWORD)&global.dwVertex1,rEBX)

                xXOR_r_rm  (rEBX,rmREG(rEBX))
           x16r xMOV_r_rm  (rBX,rmIND(rECX))
                xADD_rm_imm8(rmREG(rECX),2)
                xMOV_i32_r ((DWORD)&global.dwIndex2,rEBX)
                ILCCompile_mulvertexstride (rEBX);
                xADD_r_rm  (rEBX,rmREG(rEDX))
                xMOV_i32_r ((DWORD)&global.dwVertex2,rEBX)

                if ((ilcFlags & LUIFLAG_STRIDES) == 0x0c)
                {
                    xADD_rm_imm8(rmREG(rECX),2)
                }

                xMOV_i32_r ((DWORD)&global.pIndices,rECX)
            }

            /*
             * perform cull check
             */
            ILCCompile_cullcheck();
            label_cullbranch = ilcCount;
            xJZ32 (0); // to be patched up later

#ifdef NV_PROFILE // add result to triangle area - only if enabled
            if (nvpEnableBits & ((1 << NVP_C_TRIAREA) | (3 << NVP_C_1PIXCLK) | (1 << NVP_C_PIXPERSEC)))
            {
                xPUSH_i32   ((DWORD)&global.dwTemp)
                xMOV_rm_imm (rmREG(rEAX),(DWORD)nvProfileArea)
                xCALL_rm    (rmREG(rEAX))
            }
#endif

            /*
             * check push buffer space
             */
            ILCCompile_reserve (ilcVertexSize*3 + 2);

            /*
             * send geometry
             */
            if (ilcFlags & LUIFLAG_INDEXED)
            {
                DWORD label;

                /*
                 * vertex 0
                 */
                // setup
                xMOV_r_i32  (rEAX,(DWORD)&global.dwIndex0)
                xMOV_r_rm   (rEBX,rmREG(rEAX))
                xOR_r_i32   (rEAX,(DWORD)&global.dwVertexSN)
                xAND_rm_imm8(rmREG(rEBX),((ilcFlags & LUIFLAG_DX6) ? VC_DX6_AND : VC_DX5_AND))
                // arbitrate
                // test
                xCMP_r_rm   (rEAX,rmSIB) xSIB(rEBP,rEBX,x4) xOFS32((DWORD)&global.dwVertexCache) // cmp eax,[vc+ebx*4]
                xMOV_i32_r  ((DWORD)&global.dwIndex0,rEBX)
                label = ilcCount;
                xJZ32       (0)
                // send
                xMOV_rm_r   (rmSIB,rEAX) xSIB(rEBP,rEBX,x4) xOFS32((DWORD)&global.dwVertexCache) // mov [vc+ebx*4],eax
                xMOV_r_i32  (rEDX,(DWORD)&global.dwVertex0)
                ILCCompile_vertex (0xffffffff);
                *(DWORD*)(ilcData + label + 2) = ilcCount - (label + 6);
                /*
                 * vertex 1
                 */
                // setup
                xMOV_r_i32  (rEAX,(DWORD)&global.dwIndex1)
                xMOV_r_rm   (rEBX,rmREG(rEAX))
                xOR_r_i32   (rEAX,(DWORD)&global.dwVertexSN)
                xAND_rm_imm8(rmREG(rEBX),((ilcFlags & LUIFLAG_DX6) ? VC_DX6_AND : VC_DX5_AND))
                // arbitrate (ebx != dwIndex0)
                xCMP_r_i32   (rEBX,(DWORD)&global.dwIndex0)
                xJZ32       (label_arb11)
                *(DWORD*)(ilcData + label_arb12 + 1) = ilcCount - (label_arb12 + 5);
                // test
                xCMP_r_rm   (rEAX,rmSIB) xSIB(rEBP,rEBX,x4) xOFS32((DWORD)&global.dwVertexCache) // cmp eax,[vc+ebx*4]
                xMOV_i32_r  ((DWORD)&global.dwIndex1,rEBX)
                label = ilcCount;
                xJZ32       (0)
                // send
                xMOV_rm_r   (rmSIB,rEAX) xSIB(rEBP,rEBX,x4) xOFS32((DWORD)&global.dwVertexCache) // mov [vc+ebx*4],eax
                xMOV_r_i32  (rEDX,(DWORD)&global.dwVertex1)
                ILCCompile_vertex (0xffffffff);
                *(DWORD*)(ilcData + label + 2) = ilcCount - (label + 6);
                /*
                 * vertex 2
                 */
                // setup
                xMOV_r_i32  (rEAX,(DWORD)&global.dwIndex2)
                xMOV_r_rm   (rEBX,rmREG(rEAX))
                xOR_r_i32   (rEAX,(DWORD)&global.dwVertexSN)
                xAND_rm_imm8(rmREG(rEBX),((ilcFlags & LUIFLAG_DX6) ? VC_DX6_AND : VC_DX5_AND))
                // arbitrate (ebx != dwIndex0)
                xCMP_r_i32  (rEBX,(DWORD)&global.dwIndex0)
                xJZ32       (label_arb21)
                xCMP_r_i32  (rEBX,(DWORD)&global.dwIndex1)
                xJZ32       (label_arb21)
                *(DWORD*)(ilcData + label_arb22 + 1) = ilcCount - (label_arb22 + 5);
                // test
                xCMP_r_rm   (rEAX,rmSIB) xSIB(rEBP,rEBX,x4) xOFS32((DWORD)&global.dwVertexCache) // cmp eax,[vc+ebx*4]
                xMOV_i32_r  ((DWORD)&global.dwIndex2,rEBX)
                label = ilcCount;
                xJZ32       (0)
                // send
                xMOV_rm_r   (rmSIB,rEAX) xSIB(rEBP,rEBX,x4) xOFS32((DWORD)&global.dwVertexCache) // mov [vc+ebx*4],eax
                xMOV_r_i32  (rEDX,(DWORD)&global.dwVertex2)
                ILCCompile_vertex (0xffffffff);
                *(DWORD*)(ilcData + label + 2) = ilcCount - (label + 6);
                /*
                 * launch triangle
                 */
                xMOV_r_i32  (rEAX,(DWORD)&global.dwIndex0)
                xMOV_r_i32  (rEBX,(DWORD)&global.dwIndex1)
                xMOV_r_i32  (rECX,(DWORD)&global.dwIndex2)
                ILCCompile_launch (0xffffffff);
            }
            else
            {
                /*
                 * vertices
                 */
                xMOV_r_i32  (rEDX,(DWORD)&global.dwVertex0)
                ILCCompile_vertex (0);
                xMOV_r_i32  (rEDX,(DWORD)&global.dwVertex1)
                ILCCompile_vertex (1);
                xMOV_r_i32  (rEDX,(DWORD)&global.dwVertex2)
                ILCCompile_vertex (2);
                /*
                 * launch triangle
                 */
                ILCCompile_launch (0x00000210);
            }

            /*
             * patch up cull
             */
            *(DWORD*)(ilcData + label_cullbranch + 2) = ilcCount - (label_cullbranch + 6);

#ifdef DEBUG
            /*
             * debug code
             */
            ILCCompile_dbgFlushTriangle();
#endif

            /*
             * next
             */
            if (!(ilcFlags & LUIFLAG_INDEXED))
            {
                xMOV_r_i32 (rEAX,(DWORD)&fvfData.dwVertexStride)
                xLEA_r_rm  (rEAX,rmSIB) xSIB(rEAX,rEAX,x2)
                xADD_i32_r ((DWORD)&global.dwVertex0,rEAX)
                xADD_i32_r ((DWORD)&global.dwVertex1,rEAX)
                xADD_i32_r ((DWORD)&global.dwVertex2,rEAX)
                xADD_i32_r ((DWORD)&global.pVertices,rEAX)
            }
            break;
        }
        case 0x04: // strip
        case 0x08: // fan
        {
            BOOL  isFan = (ilcFlags & LUIFLAG_STRIDES) == 0x08;
            DWORD label_vx0;
            DWORD label_vx1;

            /*
             * translate index into vertex
             */
            if (ilcFlags & LUIFLAG_INDEXED)
            {
                xMOV_r_i32   (rECX,(DWORD)&global.pIndices)
                xMOV_r_i32   (rEDX,(DWORD)&global.pVertices)
                xXOR_r_rm    (rEBX,rmREG(rEBX))
           x16r xMOV_r_rm    (rBX,rmIND(rECX))
                xADD_rm_imm8 (rmREG(rECX),2)
                ILCCompile_mulvertexstride (rEBX);
                xMOV_i32_r   ((DWORD)&global.pIndices,rECX)
                xADD_r_rm    (rEBX,rmREG(rEDX))
                xMOV_i32_r   ((DWORD)&global.dwVertex2,rEBX)
            }

            /*
             * perform cull check
             */
            ILCCompile_cullcheck();
            label_cullbranch = ilcCount;
            xJZ32 (0) // to be patched up later

#ifdef NV_PROFILE // add result to triangle area - only if enabled
            if (nvpEnableBits & ((1 << NVP_C_TRIAREA) | (3 << NVP_C_1PIXCLK) | (1 << NVP_C_PIXPERSEC)))
            {
                xPUSH_i32   ((DWORD)&global.dwTemp)
                xMOV_rm_imm (rmREG(rEAX),(DWORD)nvProfileArea)
                xCALL_rm    (rmREG(rEAX))
            }
#endif

            /*
             * check push buffer space
             */
            ILCCompile_reserve (ilcVertexSize*3 + 2);

            /*
             * vertex 0
             */
            xMOV_r_i32    (rEBX,(DWORD)&global.dwStripFanData)
            xTEST_rm_imm  (rmREG(rEBX),0x40000)
            label_vx0 = ilcCount;
            xJNZ32        (0)

            xMOV_r_i32  (rEDX,(DWORD)&global.dwVertex0)
            xAND_rm_imm (rmREG(rEBX),0x00f)
            ILCCompile_vertex (0xffffffff);

            *(DWORD*)(ilcData + label_vx0 + 2) = ilcCount - (label_vx0 + 6);

            /*
             * vertex 1
             */
            xMOV_r_i32    (rEBX,(DWORD)&global.dwStripFanData)
            xTEST_rm_imm  (rmREG(rEBX),0x20000)
            label_vx1 = ilcCount;
            xJNZ32        (0)

            xMOV_r_i32  (rEDX,(DWORD)&global.dwVertex1)
            xAND_rm_imm (rmREG(rEBX),0x0f0)
            xSHR_rm_imm (rmREG(rEBX),4)
            ILCCompile_vertex (0xffffffff);

            *(DWORD*)(ilcData + label_vx1 + 2) = ilcCount - (label_vx1 + 6);

            /*
             * vertex 2
             */
            xMOV_r_i32  (rEBX,(DWORD)&global.dwStripFanData)
                xMOV_r_i32  (rEDX,(DWORD)&global.dwVertex2)
            xAND_rm_imm (rmREG(rEBX),0xf00)
            xSHR_rm_imm (rmREG(rEBX),8)
            ILCCompile_vertex (0xffffffff);

            /*
             * launch
             */
            xMOV_r_i32    (rEAX,(DWORD)&global.dwStripFanData)
                xMOV_r_rm     (rECX,rmREG(rEAX))
            xAND_rm_imm   (rmREG(rEAX),0xfff)
                xOR_rm_imm    (rmREG(rECX),0x70000)
                xMOV_i32_r    ((DWORD)&global.dwStripFanData,rECX)
            ILCCompile_launch (0xfffffffe);

            /*
             * patch up cull
             */
            *(DWORD*)(ilcData + label_cullbranch + 2) = ilcCount - (label_cullbranch + 6);

#ifdef DEBUG
            /*
             * debug code
             */
            ILCCompile_dbgFlushTriangle();
#endif

            /*
             * fan next tri
             */
            if (isFan)
            {
                xMOV_r_i32  (rEAX,(DWORD)&global.dwStripFanData)
                    xMOV_r_rm   (rEBX,rmREG(rEAX))
                xXOR_rm_imm (rmREG(rEAX),0x330)
                    xSHL_rm_imm (rmREG(rEBX),1)
                xAND_rm_imm (rmREG(rEAX),0x40fff)
                    xAND_rm_imm (rmREG(rEBX),0x30000)
                xOR_r_rm (rEAX,rmREG(rEBX))
                xMOV_i32_r  ((DWORD)&global.dwStripFanData,rEAX)

                xMOV_r_i32 (rEBX,(DWORD)&global.dwVertex2)
                xMOV_i32_r ((DWORD)&global.dwVertex1,rEBX)
                if (!(ilcFlags & LUIFLAG_INDEXED))
                {
                    xMOV_r_i32 (rEAX,(DWORD)&fvfData.dwVertexStride)
                    xADD_i32_r ((DWORD)&global.dwVertex2,rEAX)
                    xADD_i32_r ((DWORD)&global.pVertices,rEAX)
                }
            }
            /*
             * strip next tri
             */
            else
            {
                xMOV_rm_imm (rmREG(rEAX),0x80000000)
                xXOR_i32_r  ((DWORD)&global.dwCullMask1,rEAX)

                xMOV_r_i32  (rEAX,(DWORD)&global.dwStripFanData)
                    xMOV_r_rm   (rEBX,rmREG(rEAX))
                        xMOV_r_rm   (rECX,rmREG(rEAX))
                xSHR_rm_imm (rmREG(rEAX),4)
                    xSHL_rm_imm (rmREG(rEBX),1)
                        xSHL_rm_imm (rmREG(rECX),8)
                xAND_rm_imm (rmREG(rEAX),0x0ff)
                    xAND_rm_imm (rmREG(rEBX),0x70000)
                        xAND_rm_imm (rmREG(rECX),0xf00)
                xOR_r_rm (rEAX,rmREG(rEBX))
                xOR_r_rm (rEAX,rmREG(rECX))
                xMOV_i32_r  ((DWORD)&global.dwStripFanData,rEAX)

                xMOV_r_i32 (rEAX,(DWORD)&global.dwVertex1)
                xMOV_r_i32 (rEBX,(DWORD)&global.dwVertex2)
                xMOV_i32_r ((DWORD)&global.dwVertex0,rEAX)
                xMOV_i32_r ((DWORD)&global.dwVertex1,rEBX)
                if (!(ilcFlags & LUIFLAG_INDEXED))
                {
                    xMOV_r_i32 (rEAX,(DWORD)&fvfData.dwVertexStride)
                    xADD_i32_r ((DWORD)&global.dwVertex2,rEAX)
                    xADD_i32_r ((DWORD)&global.pVertices,rEAX)
                }
            }

            break;
        }
    }
    /*
     * prepare next primitive and loop
     * -------------------------------
     */
    xMOV_r_i32 (rEAX,(DWORD)&global.dwPrimSubCount)
    xDEC_rm    (rmREG(rEAX))
    xMOV_i32_r ((DWORD)&global.dwPrimSubCount,rEAX)
    xJNZ32     (label_draw)

    /*
     * prepare next batch and loop
     * ---------------------------
     */
    xMOV_r_i32 (rEAX,(DWORD)&global.dwPrimCount)
    xTEST_rm_r (rmREG(rEAX),rEAX)
    xJNZ32     (label_loop)

    /*
     * function exit code
     * ------------------
     */
    xMOV_i32_r ((DWORD)&nvFifo,rESI);
    xMOV_i32_r ((DWORD)&nvFreeCount,rEDI);
    xPOP_r     (rEDI)
    xPOP_r     (rESI)
    xPOP_r     (rEBP)
    xRET

    /*
     * done
     *  label_entry may never be 0
     */
    return label_entry;
}

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
        ilcData = (BYTE*)ReallocIPM(ilcData,ilcMax);
        // if this fails we are truly screwed
#ifdef DEBUG
        if (!ilcData)
        {
            DPF ("Memory allocation failed - things are going to go terribly wrong now");
            _asm int 3
        }

        memset (ilcData+ilcOld,0xcc,ilcMax-ilcOld);
#endif
    }
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
 * nvDX5TriangleSetup
 *
 * DX5 triangle setup
 */
void __stdcall nvDX5TriangleSetup
(
    DWORD  dwPrimCount,
    LPWORD pIndices,
    DWORD  dwStrides,
    LPBYTE pVertices
)
{
    /*
     * Send the context state down to the hardware.
     */
    if (pCurrentContext->dwStateChange)
        nvSetHardwareState();
    nvSetDx5TriangleState(pCurrentContext);

    /*
     * override for aa
     * aa must be enabled and we must not be capturing the push buffer
     */
    if ( (pCurrentContext->dwAntiAliasFlags & AA_ENABLED_MASK)
     && !(pCurrentContext->dwAntiAliasFlags & AA_CAPTURE_PUSHBUFFER))
    {
        pCurrentContext->dwFunctionLookup = NV_AA_FUNCTION_INDEX;
        nvAACapturePrimitive (dwPrimCount,pIndices,dwStrides,pVertices);
        return;
    }
    /*
     * calc appropriate render function
     */
    pCurrentContext->dwFunctionLookup = (pCurrentContext->dwRenderState[D3DRENDERSTATE_FOGENABLE] ? pCurrentContext->dwRenderState[D3DRENDERSTATE_FOGTABLEMODE] : 0)
#ifndef FLOAT_W
                                      | ((pCurrentContext->dwRenderState[D3DRENDERSTATE_ZENABLE] == D3DZB_USEW) ? LUIFLAG_FIXEDWBUF : 0)
#endif
                                      | ((global.dwCPUFeatureSet & FS_MMX)    ? LUIFLAG_MMX    : 0)
                                      | ((global.dwCPUFeatureSet & FS_KATMAI) ? LUIFLAG_KATMAI : 0);

#ifdef NV_CONTROLTRAFFIC
    if (pCurrentContext->dwCTFlags & NV_CT_ENABLED)
    {
        pCurrentContext->dwFunctionLookup |= (pCurrentContext->dwCTFlags & NV_CT_FRONT)
                                           ? LUIFLAG_ZFRONT
                                           : LUIFLAG_ZBACK;
    }
#endif //NV_CONTROLTRAFFIC

    return;
}

/*
 * nvDX6TriangleSetup
 *
 * DX6 triangle setup
 */
void __stdcall nvDX6TriangleSetup
(
    DWORD  dwPrimCount,
    LPWORD pIndices,
    DWORD  dwStrides,
    LPBYTE pVertices
)
{
    /*
     * setup HW if needed
     */
    if (pCurrentContext->dwStateChange)
    {
        nvSetMultiTextureHardwareState();
        if (!pCurrentContext->bUseDX6Class)
        {
            /*
             * The number of texture stages was reduced to 1.
             * Use DX5 Class instead of DX6 class.
             */
            nvDX5TriangleSetup(dwPrimCount,pIndices,dwStrides,pVertices);
            return;
        }
    }
    nvSetDx6MultiTextureState(pCurrentContext);

    /*
     * override for aa
     * aa must be enabled and we must not be capturing the push buffer
     */
    if ( (pCurrentContext->dwAntiAliasFlags & AA_ENABLED_MASK)
     && !(pCurrentContext->dwAntiAliasFlags & AA_CAPTURE_PUSHBUFFER))
    {
        pCurrentContext->dwFunctionLookup = NV_AA_FUNCTION_INDEX;
        nvAACapturePrimitive (dwPrimCount,pIndices,dwStrides,pVertices);
        return;
    }

    /*
     * calc appropriate render function
     */
    pCurrentContext->dwFunctionLookup = (pCurrentContext->dwRenderState[D3DRENDERSTATE_FOGENABLE] ? pCurrentContext->dwRenderState[D3DRENDERSTATE_FOGTABLEMODE] : 0)
#ifndef FLOAT_W
                                      | ((pCurrentContext->dwRenderState[D3DRENDERSTATE_ZENABLE] == D3DZB_USEW) ? LUIFLAG_FIXEDWBUF : 0)
#endif
                                      | ((global.dwCPUFeatureSet & FS_MMX)    ? LUIFLAG_MMX    : 0)
                                      | ((global.dwCPUFeatureSet & FS_KATMAI) ? LUIFLAG_KATMAI : 0)
                                      | LUIFLAG_DX6
                                      | LUIFLAG_FVF;

#ifdef NV_CONTROLTRAFFIC
    if (pCurrentContext->dwCTFlags & NV_CT_ENABLED)
    {
        pCurrentContext->dwFunctionLookup |= (pCurrentContext->dwCTFlags & NV_CT_FRONT)
                                           ? LUIFLAG_ZFRONT
                                           : LUIFLAG_ZBACK;
    }
#endif //NV_CONTROLTRAFFIC

    return;
}


/*
 * nvTriangleDispatch(dwPrimCount,NULL,LEGACY_STRIDES,(LPBYTE)lpVertices);
 *
 * calls the triangle inner loop code
 *  - if it does not exist yet, build it
 */
void __stdcall nvTriangleDispatch
(
    DWORD  dwPrimCount,
    LPWORD pIndices,
    DWORD  dwStrides,
    LPBYTE pVertices
)
{
    DWORD dwFLUI;
    DWORD dwDrawPrim;

    if (!dwPrimCount) return; // do not waste time (or crash code that assume real work to be done)

    /*
    {
        extern BOOL nvPusherDisassemblerEnable;
        nvPusherDisassemblerEnable = 1;
    }
    */

    /*
     * get function pointer
     */
    dwFLUI = pCurrentContext->dwFunctionLookup;

    /*
     * if we have to call a fixed function, do it now
     *  current fixed functions are
     *      nvTriangleSetup
     *      nvAACapturePrimitive
     */
    if ((dwFLUI & LUIMASK_BRANCH) >= NV_FIXED_FUNCTION_INDEX)
    {
        switch (dwFLUI)
        {
            case NV_AA_FUNCTION_INDEX:
            {
                nvAACapturePrimitive (dwPrimCount,pIndices,dwStrides,pVertices);
                return;
            }
            case NV_SETUP_FUNCTION_INDEX:
            {
                (pCurrentContext->bUseDX6Class ? nvDX6TriangleSetup
                                               : nvDX5TriangleSetup)(dwPrimCount,pIndices,dwStrides,pVertices);

                /*
                 * reset dwFLUI
                 */
                dwFLUI = pCurrentContext->dwFunctionLookup;

                /*
                 * fall through so we can draw the primitives except for AA
                 */
                if ((dwFLUI & LUIMASK_BRANCH) >= NV_FIXED_FUNCTION_INDEX) return;


                break;
            }
        }
    }

#ifdef NV_TEX2
    /*
     * tell texture manager that we are now going to use textures
     * the calls may modify state so that we have to redo HW
     */
    if (pCurrentContext->pTexture0 || pCurrentContext->pTexture1)
    {
        if (pCurrentContext->pTexture0)
            nvTextureRef (pCurrentContext->pTexture0);
        if (pCurrentContext->pTexture1)
            nvTextureRef (pCurrentContext->pTexture1);
        /*
         * if the above changed state - update now
         * yes, it is possible to call setup twice but this happens very infrequently
         */
        (pCurrentContext->bUseDX6Class ? nvDX6TriangleSetup
                                       : nvDX5TriangleSetup)(dwPrimCount,pIndices,dwStrides,pVertices);

        /*
         * reset dwFLUI
         */
        dwFLUI = pCurrentContext->dwFunctionLookup;
    }
#endif

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
    dwFLUI |= ( pIndices                                 ? LUIFLAG_INDEXED : 0)
           |  ((fvfData.dwVertexType != D3DFVF_TLVERTEX) ? LUIFLAG_FVF     : 0)
           |  ((dwStrides >> 24) << 2);

    /*
     * (dbgLevel & NVDBG_LEVEL_FASTLOOPS) will tell the world what we did here
     */
#if defined(DEBUG) || defined(PRINT_NAME)
    {
        static char *szFog[] = {"","ExpFog","Exp2Fog","LinFog"};
        static char *szPMT[] = {"List","Strip","Fan","Legacy"};
        static char *szIdx[] = {"Ordered","Indexed"};
        static char *szFVF[] = {"","FVF"};
        static char *szDX6[] = {"DX5","DX6"};
        static char *szFWB[] = {"","FixedWB"};
        static char *szMMX[] = {"","_MMX"};
        static char *szKTM[] = {"","_KATMAI"};
        char sz[512];
        strcpy (sz,"nvTriangleDispatch: ");
        strcat (sz,szDX6[(dwFLUI & LUIFLAG_DX6)       ? 1 : 0]);
        strcat (sz,szFVF[(dwFLUI & LUIFLAG_FVF)       ? 1 : 0]);
        strcat (sz,szFog[dwFLUI & LUIFLAG_FOGMODE]);
        strcat (sz,szIdx[(dwFLUI & LUIFLAG_INDEXED)   ? 1 : 0]);
        strcat (sz,"Triangle");
        strcat (sz,szPMT[(dwFLUI & LUIFLAG_STRIDES)>>2]);
        strcat (sz,szFWB[(dwFLUI & LUIFLAG_FIXEDWBUF) ? 1 : 0]);
        strcat (sz,szMMX[(dwFLUI & LUIFLAG_MMX)       ? 1 : 0]);
        strcat (sz,szKTM[(dwFLUI & LUIFLAG_KATMAI)    ? 1 : 0]);
        strcat (sz," (dwFLUI = %02x)");
#ifdef PRINT_NAME
        {
            char sz2[256];
            wsprintf (sz2,sz,dwFLUI);
            strcat (sz2,"\n");
            OutputDebugString (sz2);
        }
#else
        DPF_LEVEL (NVDBG_LEVEL_FASTLOOPS,sz,dwFLUI);
#endif
    }
#endif // DEBUG

    /*
     * get fptr
     */
    dwDrawPrim = dwDrawPrimitiveTable[dwFLUI & LUIMASK_BRANCH];

    /*
     * compile it if it does not exist
     */
    if (!dwDrawPrim)
    {
#ifdef NV_PROFILE
        NVP_START (NVP_T_BUILDLOOP);
#endif
        dwDrawPrim                                    = ILCCompileCode(dwFLUI);
        dwDrawPrimitiveTable[dwFLUI & LUIMASK_BRANCH] = dwDrawPrim;
#ifdef NV_PROFILE
        NVP_STOP (NVP_T_BUILDLOOP);
        nvpLogTime (NVP_T_BUILDLOOP,nvpTime[NVP_T_BUILDLOOP]);
#endif
    }

#ifdef NV_PROFILE
    global.fTriangleArea = 0.0f;
    NVP_START (NVP_T_INNERLOOP);
#endif
    /*
     * setup inner loop variables
     */
    global.dwPrimCount = dwPrimCount;
    global.dwStrides   = dwStrides;
    global.pVertices   = (DWORD)pVertices;
    global.pIndices    = (DWORD)pIndices;
    global.dwCullMask1 = (pCurrentContext->dwRenderState[D3DRENDERSTATE_CULLMODE] == D3DCULL_CCW)  ? 0x80000000 : 0x00000000;
    global.dwCullMask2 = (pCurrentContext->dwRenderState[D3DRENDERSTATE_CULLMODE] == D3DCULL_NONE) ? 0xffffffff : 0x80000000;
    if (dwFLUI & LUIFLAG_FVF)
    {
        PNVD3DMULTITEXTURESTATE pTriangleState = (PNVD3DMULTITEXTURESTATE)&pCurrentContext->mtsState;
        global.pdwUVOffset = pTriangleState->dwUVOffset;
    }
    if (dwFLUI & LUIFLAG_FOGMODE)
    {
        global.fFogTableDensity     = pCurrentContext->fFogTableDensity;
        global.fFogTableLinearScale = pCurrentContext->fFogTableLinearScale;
        global.fFogTableEnd         = pCurrentContext->fFogTableEnd;
    }
    if (dwFLUI & LUIFLAG_FIXEDWBUF)
    {
        global.fRWFar               = pCurrentContext->dvRWFar;
    }
    if (dwFLUI & LUIFLAG_DX6)
    {
    }
    else
    {
        global.dwSpecularState = pCurrentContext->dwRenderState[D3DRENDERSTATE_SPECULARENABLE];
        global.dwSpecCurr      = pCurrentContext->ctxInnerLoop.dwCurrentSpecularState;
        global.dwBlend         = pCurrentContext->ctxInnerLoop.dwCurrentBlend;
        global.dwSpecCount     = pCurrentContext->ctxInnerLoop.dwNoSpecularTriangleCount;
    }

    global.dwWMask1 = ((pCurrentContext->dvWNear == 1.0f) && (pCurrentContext->dvWFar  == 1.0f)) ? 0xffffffff : 0;
    global.dwWMask2 = ~global.dwWMask1;

#ifdef NV_PROFILE
    if (nvpEnableBits & (1 << NVP_C_PIXPERSEC))
    {
        nvpTriArea  = 0.0f;
        nvpTriCount = 0;
        nvpTriToGo  = nvpTriPerSecWindow;
        NVP_START (NVP_C_PIXPERSEC);
    }
#endif

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


        while (nvFreeCount < dwCount)
            nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, dwCount);

        ((DWORD*)nvFifo)[0] = dwCode + dwBase * dwSize;
        memcpy ((void*)(nvFifo + 4),&vx0,dwSize);
        nvFifo      += (dwSize + 4);
        nvFreeCount -= (dwSize + 4) / 4;

        dwBase ++;

        ((DWORD*)nvFifo)[0] = dwCode + dwBase * dwSize;
        memcpy ((void*)(nvFifo + 4),&vx1,dwSize);
        nvFifo      += (dwSize + 4);
        nvFreeCount -= (dwSize + 4) / 4;

        dwBase ++;

        ((DWORD*)nvFifo)[0] = dwCode + dwBase * dwSize;
        memcpy ((void*)(nvFifo + 4),&vx2,dwSize);
        nvFifo      += (dwSize + 4);
        nvFreeCount -= (dwSize + 4) / 4;
    }
#endif //PIX1

    /*
     * draw it
     */
    ((PFNINNERLOOP)(ilcData + dwDrawPrim))();

#ifdef NV_PROFILE
    if (nvpEnableBits & (1 << NVP_C_PIXPERSEC))
    {
        nvpTriToGo = 1;
        nvProfileLog(); // force tri/sec dump here
    }
#endif

    /*
     * finish loop variables
     */
    if (dwFLUI & LUIFLAG_DX6)
    {
    }
    else
    {
         pCurrentContext->ctxInnerLoop.dwNoSpecularTriangleCount = global.dwSpecCount;
         pCurrentContext->ctxInnerLoop.dwCurrentSpecularState    = global.dwSpecCurr;
         pCurrentContext->ctxInnerLoop.dwCurrentBlend            = global.dwBlend;
    }

    /*
     * update CT bits
     */
#ifdef NV_CONTROLTRAFFIC
    pCurrentContext->dwCTFlags |= NV_CT_DRAWPRIM;
#endif //NV_CONTROLTRAFFIC

#ifdef NV_PROFILE
    NVP_STOP (NVP_T_INNERLOOP);
    nvpLogTime (NVP_T_INNERLOOP,nvpTime[NVP_T_INNERLOOP]);
#endif

#ifdef NV_PROFILE
    /*
     * pixel per clock calculations
     */
    if (nvpEnableBits & (2 << NVP_C_1PIXCLK))
    {
        DWORD dwPixelsPerClock;

        if (dwFLUI & LUIFLAG_DX6)
        {
            dwPixelsPerClock = 1;
        }
        else
        {

            DWORD drfValueFilter;
            DWORD drfValueSpec;
            DWORD drfValueAlpha;
            DWORD drfValueFog;
            drfValueFilter = DRF_VAL(054, _FILTER, _TEXTUREMIN, pCurrentContext->ctxInnerLoop.dwFilter);
            drfValueSpec = DRF_VAL(054, _BLEND, _SPECULARENABLE, pCurrentContext->ctxInnerLoop.dwBlend);
            drfValueAlpha = DRF_VAL(054, _BLEND, _ALPHABLENDENABLE, pCurrentContext->ctxInnerLoop.dwBlend);
            drfValueFog = DRF_VAL(054, _BLEND, _FOGENABLE, pCurrentContext->ctxInnerLoop.dwBlend);

            if ((drfValueFilter == NV054_FILTER_TEXTUREMIN_LINEARMIPNEAREST) ||
                (drfValueFilter == NV054_FILTER_TEXTUREMIN_LINEARMIPLINEAR)) {
                dwPixelsPerClock = 1;

            } else if (drfValueSpec == NV054_BLEND_SPECULARENABLE_TRUE) {
                dwPixelsPerClock = 1;

            } else if (drfValueAlpha == NV054_BLEND_ALPHABLENDENABLE_TRUE &&
                       drfValueFog == NV054_BLEND_FOGENABLE_TRUE) {
                dwPixelsPerClock = 1;
            } else {
                dwPixelsPerClock = 2;
            }
        }

        nvpLogEvent (NVP_E_TRIDISP);
        switch (dwPixelsPerClock)
        {
            case 1:
            {
                nvpLogCount (NVP_C_1PIXCLK,(DWORD)(0.5f * global.fTriangleArea + 0.5f));
                break;
            }
            case 2:
            {
                nvpLogCount (NVP_C_2PIXCLK,(DWORD)(0.5f * global.fTriangleArea + 0.5f));
                break;
            }
        }
    }
#endif // NV_PROFILE

    /*
    {
        extern BOOL nvPusherDisassemblerEnable;
        NV_D3D_GLOBAL_SAVE();
        nvFlushDmaBuffers();
        NV_D3D_GLOBAL_SETUP();
        nvPusherDisassemblerEnable = 0;
    }
    */

}

#ifdef NV_DUMPLOOPS
void __cdecl print (HANDLE f,char *sz,...)
{
    char buf[512];
    DWORD dw;
    vsprintf (buf,sz,(char*)(((DWORD)&sz)+4));
    WriteFile (f,buf,strlen(buf),&dw,0);
}
#endif

#ifdef NV_EMBEDLOOPS   // -DNV_EMBEDLOOPS -Zm999
#include "\loops.c"
#endif

/*
 * ILCWarmCache
 *
 * loads the most common inner loops
 */
void ILCWarmCache
(
    void
)
{
    DWORD dwDefault = ((global.dwCPUFeatureSet & FS_MMX)    ? LUIFLAG_MMX    : 0)
                    | ((global.dwCPUFeatureSet & FS_KATMAI) ? LUIFLAG_KATMAI : 0);

#ifndef NV_EMBEDLOOPS
#ifndef NV_DUMPLOOPS
#ifndef NV_PROFILE // we do not prebuild code for the profiler since we read the enable bits too late
    DWORD dwFLUI;

    /*
     * dx5 TL verts
     */
    dwFLUI = dwDefault | 0x0000; // list
    dwDrawPrimitiveTable[dwFLUI & LUIMASK_BRANCH] = ILCCompileCode(dwFLUI);
    dwFLUI = dwDefault | 0x0004; // strip
    dwDrawPrimitiveTable[dwFLUI & LUIMASK_BRANCH] = ILCCompileCode(dwFLUI);
    dwFLUI = dwDefault | 0x0008; // fan
    dwDrawPrimitiveTable[dwFLUI & LUIMASK_BRANCH] = ILCCompileCode(dwFLUI);
    dwFLUI = dwDefault | 0x000c; // legacy
    dwDrawPrimitiveTable[dwFLUI & LUIMASK_BRANCH] = ILCCompileCode(dwFLUI);

    /*
     * dx6 fvf verts
     */
    dwFLUI = dwDefault | 0x0000 | LUIFLAG_FVF | LUIFLAG_DX6; // list
    dwDrawPrimitiveTable[dwFLUI & LUIMASK_BRANCH] = ILCCompileCode(dwFLUI);
    dwFLUI = dwDefault | 0x0004 | LUIFLAG_FVF | LUIFLAG_DX6; // strip
    dwDrawPrimitiveTable[dwFLUI & LUIMASK_BRANCH] = ILCCompileCode(dwFLUI);
    dwFLUI = dwDefault | 0x0008 | LUIFLAG_FVF | LUIFLAG_DX6; // fan
    dwDrawPrimitiveTable[dwFLUI & LUIMASK_BRANCH] = ILCCompileCode(dwFLUI);
    dwFLUI = dwDefault | 0x000c | LUIFLAG_FVF | LUIFLAG_DX6; // legacy
    dwDrawPrimitiveTable[dwFLUI & LUIMASK_BRANCH] = ILCCompileCode(dwFLUI);
#endif
#endif
#endif

#ifdef NV_DUMPLOOPS
    {
        DWORD  i;
        HANDLE f;

        /*
         * build all possible inner loops
         */
        for (i=0; i<256; i++)
        {
            dwDrawPrimitiveTable[i] = ILCCompileCode(i | dwDefault);
        }

        /*
         * save them to disk
         */
        f = CreateFile("\\loops.c",GENERIC_WRITE,0,0,CREATE_ALWAYS,0,0);
        if (f != INVALID_HANDLE_VALUE)
        {
            // max
            print (f,"DWORD embedMax = %d;",ilcMax);
            print (f,"\r\n\r\n");
            // data
            print (f,"void embedData (void) {\r\n");
            for (i = 0; i < ilcCount; i++)
            {
                if ((i & 31) ==  0) print (f,"    ");
                print (f,"__asm _emit 0x%02x ",ilcData[i]);
                if ((i & 31) == 31) print (f,"\r\n");
            }
            print (f,"}\r\n\r\n");
            CloseHandle (f);
        }
        else
        {
            OutputDebugString ("File creation failure\n");
            __asm int 3;
        }
    }
#endif

#ifdef NV_EMBEDLOOPS
    {
        DWORD i;

        ilcData  = (BYTE*)embedData;
        ilcMax   = embedMax;
        ilcCount = 0;

        for (i=0; i<256; i++)
        {
            dwDrawPrimitiveTable[i] = ILCCompileCode(i | dwDefault);
        }
    }
#endif
}

#endif //NV_FASTLOOPS