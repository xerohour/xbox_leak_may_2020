 /***************************************************************************\
|*                                                                           *|
|*        Copyright (c) 1993-1999 NVIDIA, Corp.  All rights reserved.        *|
|*                                                                           *|
|*     NOTICE TO USER:   The source code  is copyrighted under  U.S. and     *|
|*     international laws.   NVIDIA, Corp. of Sunnyvale, California owns     *|
|*     the copyright  and as design patents  pending  on the design  and     *|
|*     interface  of the NV chips.   Users and possessors of this source     *|
|*     code are hereby granted  a nonexclusive,  royalty-free  copyright     *|
|*     and  design  patent license  to use this code  in individual  and     *|
|*     commercial software.                                                  *|
|*                                                                           *|
|*     Any use of this source code must include,  in the user documenta-     *|
|*     tion and  internal comments to the code,  notices to the end user     *|
|*     as follows:                                                           *|
|*                                                                           *|
|*     Copyright (c) 1993-1997  NVIDIA, Corp.    NVIDIA  design  patents     *|
|*     pending in the U.S. and foreign countries.                            *|
|*                                                                           *|
|*     NVIDIA, CORP.  MAKES  NO REPRESENTATION ABOUT  THE SUITABILITY OF     *|
|*     THIS SOURCE CODE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT     *|
|*     EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORP. DISCLAIMS     *|
|*     ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,  INCLUDING  ALL     *|
|*     IMPLIED   WARRANTIES  OF  MERCHANTABILITY  AND   FITNESS   FOR  A     *|
|*     PARTICULAR  PURPOSE.   IN NO EVENT SHALL NVIDIA, CORP.  BE LIABLE     *|
|*     FOR ANY SPECIAL, INDIRECT, INCIDENTAL,  OR CONSEQUENTIAL DAMAGES,     *|
|*     OR ANY DAMAGES  WHATSOEVER  RESULTING  FROM LOSS OF USE,  DATA OR     *|
|*     PROFITS,  WHETHER IN AN ACTION  OF CONTRACT,  NEGLIGENCE OR OTHER     *|
|*     TORTIOUS ACTION, ARISING OUT  OF OR IN CONNECTION WITH THE USE OR     *|
|*     PERFORMANCE OF THIS SOURCE CODE.                                      *|
|*                                                                           *|
 \***************************************************************************/

/**************************************************************************************************************
*
*	Module: nv15mem.c
*
*	Description:
*		Device-specific memory sizing code, for Nv15 and NV11.
*
*	Revision History:
*		Original -- 1/00 Jeff Westerinen
*
**************************************************************************************************************/

#ifdef NV_PORTABLE

/* "portable" form for use outside resman and by external source customers */
/* look in devinit/portable/ for more details */
#include <nv_portable.h>
#include <nv_ref.h>
#include <nv10_ref.h>
#include <devinit.h>

#else	/* not NV_PORTABLE --> Nvidia internal */

#include <nvrm.h>
#include <nvhw.h>
#include <nv10_ref.h>
#include <os.h>
#include <devinit.h>

#endif

RM_STATUS Nv15MemConfigureClocks
(
    PHWINFO pDev,
    MEM_INIT_TABLE_NV15 memInitTable
)
{
    RM_STATUS rmStatus = RM_OK;
    U032 clk;
    U032 memInitTableIndex;
    U032 M,N,P;
    U032 Head = 0;  // MPLL & NVPLL are on Head 0

    // get the index in the memory init table from scratch register 4
    CRTC_RD(NV_CIO_CRE_SCRATCH4__INDEX, memInitTableIndex, 0);
    memInitTableIndex >>= 4;
    
    // program NVCLK
    clk = memInitTable[memInitTableIndex].NVClk_PLL;
    dacCalcMNP(pDev, Head, clk, &M, &N, &P);
    REG_WR32(
        NV_PRAMDAC_NVPLL_COEFF, 
        DRF_NUM(_PRAMDAC, _NVPLL_COEFF, _MDIV, M) |
            DRF_NUM(_PRAMDAC, _NVPLL_COEFF, _NDIV, N) |
            DRF_NUM(_PRAMDAC, _NVPLL_COEFF, _PDIV, P)
    );
                                        
    // program MCLK -- double the clock if flags indicate DDR memory
    clk = memInitTable[memInitTableIndex].MClk_PLL;
    if (memInitTable[memInitTableIndex].Flags == DDR)
    {
        clk *= 2;
    }
    dacCalcMNP(pDev, Head, clk, &M, &N, &P);
    REG_WR32(
        NV_PRAMDAC_MPLL_COEFF, 
        DRF_NUM(_PRAMDAC, _MPLL_COEFF, _MDIV, M) |
            DRF_NUM(_PRAMDAC, _MPLL_COEFF, _NDIV, N) |
            DRF_NUM(_PRAMDAC, _MPLL_COEFF, _PDIV, P));
            
    return rmStatus;
    
} // end of Nv15MemConfigureClocks()

    
RM_STATUS Nv15MemConfigureMemory
(
    PHWINFO pDev,
    MEM_INIT_TABLE_NV15 memInitTable, 
    U032 SDRSeqTable[], 
    U032 DDRSeqTable[]
)
{
    RM_STATUS rmStatus = RM_OK;
    U032* seqTable;
    U032 memInitTableIndex, initValue;
    U032 memInitDataIndex = 0; 
    U032 seqTableIndex = 0;
    
    
    
    // get the index in the memory init table from scratch register 4
    CRTC_RD(NV_CIO_CRE_SCRATCH4__INDEX, memInitTableIndex, 0);
    memInitTableIndex >>= 4;
    
    // set proper sequence table
    switch (memInitTable[memInitTableIndex].Flags)
    {
        case SDR:
            seqTable = SDRSeqTable;
            break;
            
        case DDR:
            seqTable = DDRSeqTable;
            break;
            
        default:
            return RM_ERROR;
    }
    
    // execute the memory init sequence
    while (seqTable[seqTableIndex] != MEM_INIT_END_OF_SEQUENCE)
    {
        switch (seqTable[seqTableIndex])
        {
            case NV_PFB_PRE:
	            FLD_WR_DRF_DEF(_PFB,_PRE,_CMD,_PRECHARGE);
                break;
                
            case NV_PFB_PAD:
	            FLD_WR_DRF_DEF(_PFB,_PAD,_CKE,_NORMAL);
                break;
                
            case NV_PFB_REF:
	            FLD_WR_DRF_DEF(_PFB,_REF,_CMD,_REFRESH);
                break;
                
            case NV_PFB_EMRS:
                if ((initValue = memInitTable[memInitTableIndex].data[memInitDataIndex++]) != 0xffffffff)
                {
                    REG_WR32(NV_PFB_EMRS, initValue);
                }
                break;
                
            default:
                REG_WR32(seqTable[seqTableIndex], memInitTable[memInitTableIndex].data[memInitDataIndex++]);
                break;
        }
        seqTableIndex++;
    }
    
    return rmStatus;
    
} // end of Nv15MemConfigureMemory()



// end of nv15mem.c

