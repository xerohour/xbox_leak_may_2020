 /***************************************************************************\
|*                                                                           *|
|*        Copyright (c) 1993-2000 NVIDIA, Corp.  All rights reserved.        *|
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
|*     Copyright (c) 1993-2000  NVIDIA, Corp.    NVIDIA  design  patents     *|
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

/****************************************************************************\
*                                                                           *
* Module: C04DNV20.C                                                        *
*   This module implements the chip-dependent portion of the                *
*   NV_EXTERNAL_VIDEO_DECODER object class and its corresponding methods.   *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/
#include <nv_ref.h>
#include <nv20_ref.h>
#include <nvrm.h>
#include <nv20_hal.h>
#include "nvhw.h"
#include "nvhalcpp.h"

//
// The following routines are used by the HAL mediaport engine
// manager in kernel/mp/nv20/mpnv20.c.
//
RM_STATUS class04DMethod_NV20(PMPMETHODARG_000);
RM_STATUS class04DGetEventStatus_NV20(PMPGETEVENTSTATUSARG_000);
RM_STATUS class04DServiceEvent_NV20(PMPSERVICEEVENTARG_000);

#define Y_SCALE_INCR_PRECISION  10

//---------------------------------------------------------------------------
//
//  Create/Destroy object routines.
//
//---------------------------------------------------------------------------

//
// Class instantiation/destruction is handled by mp/mpobj.c and
// mp/nv20/mpnv20.c.
//

//---------------------------------------------------------------------------
//
//  Miscellaneous class support routines.
//
//---------------------------------------------------------------------------

#if 0
static RM_STATUS class04DFindNearestScaleFactor(
    U032 dwRealDivisor,
    U032 *dwSpecialIndex
)
{
    U032 dwIndex=0;
    U032 X_ScaleFactors[] = { 1,2,3,4,6,8,12,16,24, 0xFFFF };

    //
    // Converts dwRealDivisor into an index which will point to nearest best quality divisor,
    //  will always be greater than RealDivisor if not equal
    //
    *dwSpecialIndex = 0xFFFF;
    
    while (X_ScaleFactors[dwIndex] != 0xFFFF)
    {
    	if (dwRealDivisor >= X_ScaleFactors[dwIndex])
    		*dwSpecialIndex = dwIndex;
    	dwIndex++;
    }	

    // this should never occur
    if (*dwSpecialIndex == 0xFFFF)
    	{
    	DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: class04DFindNearestScaleFactor, cannot find nearest scale factor !\r\n");
    	*dwSpecialIndex = 1; // could not find index so just do a verbatim copy
        return (RM_ERROR);
    	}

    return (RM_OK);
}
#endif

// these are the weights supplied by Al Z
// they are derived from files located here:
// /home/al/proj/nv10/cmodel/video
// filb5_8.fil filc5_8.fil fild5_8.fil file5_8.fil
//  filb5_8.fil  corresponds to the b's section

static S032 VPFilterCos[5][8][5] = {
    { {0, 0, 128, 0, 0 },             // a's
      {0, 0, 128, 0, 0 },
      {0, 0, 128, 0, 0 },
      {0, 0, 128, 0, 0 },
      {0, 0, 128, 0, 0 },
      {0, 0, 128, 0, 0 },
      {0, 0, 128, 0, 0 },
      {0, 0, 128, 0, 0 }  },
      
    { {-14,  78,  78, -14,  0},      //  b's
      {-10,  50, 101, -20,  7},
      { -3,  25, 123, -22,  5},
      {  2,   4, 135, -19,  6},
      {  5, -11, 140, -11,  5},
      {  6, -19, 135,   4,   2},
      {  5, -22, 123,  25, -3},
      {  7, -20, 101,  50, -10} },
      
    { {-10, 74, 74, -10,  0 },     //c's
      {-13, 60, 86,  -7,  2 },
      {-11, 48, 96,   1, -6 },
      {-10, 33,101,  10, -6 },
      { -8, 20,104,  20, -8 },
      {-6,  10,101,  33, -10},
      {-6,   1, 96,  48, -11},
      {  2, -7, 86, 60, -13 }  },
      
    { { 5, 59, 59,  5,  0},       // d's
      { 4, 55, 66, 12, -9},
      {-1, 47, 68, 18, -4},
      {-3, 39, 72, 24, -4},
      {-4, 32, 72, 32, -4},
      {-4, 24, 72, 39, -3},
      {-4, 18, 68, 47, -1},
      {-9, 12, 66, 55,  4} },
      
    { {22, 42, 42, 22, 0 },    // e's
      {18, 39, 43, 24, 6 },
      {15, 37, 44, 26, 6 },
      {12, 35, 44, 29, 8 },
      {10, 32, 44, 32, 10},
      { 8, 29, 44, 35, 12},
      { 6, 26, 44, 37, 15},
      { 4, 24, 43, 39, 18}  }
   };
      
static RM_STATUS class04DProgramWeights(
    PHALHWINFO pHalHwInfo,
    U032 WidthIn,
    U032 WidthOut
)
{
    U032        scalefactor;
    U032        chunkIndex;
    U032        i;
    U032        ACos;
    U032        BCos;
    U008        dummy1;
    U008        dummy2;
    U008        dummy3;
 
    /*
     * hardware does not support upscaling
     */

    if ( WidthOut > WidthIn ) {
        return(RM_ERROR);
    }
    
    if(WidthIn == 0 ) {
        // WidthIn is used to indicate "empty" capture
        // so don't reprogram the weights...
        return (RM_OK);
    }
    
    if((WidthOut == 0) || (WidthIn == 0)) {
      scalefactor = 65536;
    } else {
      scalefactor = WidthIn * 65536 / WidthOut;
    }
    
    if( scalefactor > 163840 ) { // if( scalefactor > 2.5 * ( 1<< 16) ) 
        chunkIndex = 4;  //use e
    } 
    else if( scalefactor > 100270 ) { // else if( scalefactor > 1.53 * ( 1<< 16) ) 
        chunkIndex = 3;  //use d
    } 
    else if( scalefactor > 76677 ) { // else if( scalefactor > 1.17 * ( 1<< 16) )
        chunkIndex = 2;  //use c
    } 
    else if( scalefactor > 65536 ) { // else if( scalefactor > 1.0 * ( 1<< 16) ) 
        chunkIndex = 1;  //use b
    } 
    else {
        chunkIndex = 0;  //use a
    } 

    for(i=0;i<8;i++ ) {
        dummy1 = (U008)VPFilterCos[chunkIndex][i][0];
        dummy2 = (U008)VPFilterCos[chunkIndex][i][1];
        dummy3 = (U008)VPFilterCos[chunkIndex][i][2];
        ACos = 
            ((((unsigned long) dummy3 ) << 16 ) & 0xFF0000 ) |
            ((((unsigned long) dummy2 ) << 8  ) & 0x00FF00 ) |
            ((((unsigned long) dummy1 ) << 0  ) & 0x0000FF );
        
        dummy1 = (U008)VPFilterCos[chunkIndex][i][3];
        dummy2 = (U008)VPFilterCos[chunkIndex][i][4];
        BCos = 
            ((((unsigned long) dummy2 ) << 8  ) & 0x00FF00 ) |
            ((((unsigned long) dummy1 ) << 0  ) & 0x0000FF );

        REG_WR32(NV_PME_HORIZ_WGHTS_A(i),ACos);
        REG_WR32(NV_PME_HORIZ_WGHTS_B(i),BCos);
    }

    return (RM_OK);
}
        
static RM_STATUS class04DStartVbi
(
    PHALHWINFO pHalHwInfo,
    PVIDEODECODERHALOBJECT pDecoderHalObj,
    U032 Buffer
)
{
    V032 stateSU, stateME;

    //DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class04DStartVbi\r\n");

    //
    // Make sure the engine is configured correctly
    //
    FLD_WR_DRF_DEF(_PME, _656_CONFIG, _ANC_MODE, _VBI2);
    FLD_WR_DRF_DEF(_PME, _INTR_EN_0, _ANC_NOTIFY, _ENABLED);
    
    //
    // The hardware can only support vbi dma transfers to framebuffer
    // addresses.  If this transfer is not completely contiguous,
    // we need to break the transfer up.
    //
    if (pDecoderHalObj->SetVbi[Buffer].CtxDma->AddressSpace != ADDR_FBMEM)
        return (RM_ERROR);
    
    //
    // Load up the buffer location and sizes
    //
    if (Buffer == 0)
    {
        REG_WR32(NV_PME_ANC_BUFF0_START, pDecoderHalObj->SetVbi[0].Offset
                                       + (pDecoderHalObj->SetVbi[0].CtxDma->PteArray[0] & 0xFFFFF000)
                                       + pDecoderHalObj->SetVbi[0].CtxDma->PteAdjust);

        REG_WR32(NV_PME_ANC_BUFF0_PITCH, pDecoderHalObj->SetVbi[0].Pitch);
        REG_WR32(NV_PME_ANC_BUFF0_LENGTH, pDecoderHalObj->SetVbi[0].Pitch * pDecoderHalObj->SetVbi[0].Height);
    } 
    else
    {
        REG_WR32(NV_PME_ANC_BUFF1_START, pDecoderHalObj->SetVbi[1].Offset
                                       + (pDecoderHalObj->SetVbi[1].CtxDma->PteArray[0] & 0xFFFFF000)
                                       + pDecoderHalObj->SetVbi[1].CtxDma->PteAdjust);

        REG_WR32(NV_PME_ANC_BUFF1_PITCH, pDecoderHalObj->SetVbi[1].Pitch);
        REG_WR32(NV_PME_ANC_BUFF1_LENGTH, pDecoderHalObj->SetVbi[1].Pitch * pDecoderHalObj->SetVbi[1].Height);
    } 
    
    //
    // Program the global VBI settings
    //
    FLD_WR_DRF_NUM(_PME, _VBI_REGION, _START_LINE, pDecoderHalObj->SetVbi[1].FirstLine);
    FLD_WR_DRF_NUM(_PME, _VBI_REGION, _NUM_LINES, pDecoderHalObj->SetVbi[1].Height);
    
    //
    // Start the transfer
    //
    stateSU = REG_RD32(NV_PME_ANC_SU_STATE);
    stateME = REG_RD32(NV_PME_ANC_ME_STATE);
    
    if (Buffer == 0)
    {
        //
        // Choose the capture field (progressive == even)
        //
        if ((pDecoderHalObj->SetVbi[0].Field == NV04D_SET_VBI_FORMAT_FIELD_PROGRESSIVE)
         || (pDecoderHalObj->SetVbi[0].Field == NV04D_SET_VBI_FORMAT_FIELD_EVEN_FIELD))
            stateSU |= 1 << SF_SHIFT(NV_PME_ANC_SU_STATE_BUFF0_FIELD);
        else            
            stateSU &= ~(1 << SF_SHIFT(NV_PME_ANC_SU_STATE_BUFF0_FIELD));
    
        stateSU ^= 1 << SF_SHIFT(NV_PME_ANC_SU_STATE_BUFF0_IN_USE);
        REG_WR32(NV_PME_ANC_SU_STATE, stateSU);
    } 
    else // Buffer == 1
    {
        //
        // Choose the capture field (progressive == even)
        //
        if ((pDecoderHalObj->SetVbi[1].Field == NV04D_SET_VBI_FORMAT_FIELD_PROGRESSIVE)
         || (pDecoderHalObj->SetVbi[1].Field == NV04D_SET_VBI_FORMAT_FIELD_EVEN_FIELD))
            stateSU |= 1 << SF_SHIFT(NV_PME_ANC_SU_STATE_BUFF1_FIELD);
        else            
            stateSU &= ~(1 << SF_SHIFT(NV_PME_ANC_SU_STATE_BUFF1_FIELD));
    
        stateSU ^= 1 << SF_SHIFT(NV_PME_ANC_SU_STATE_BUFF1_IN_USE);
        REG_WR32(NV_PME_ANC_SU_STATE, stateSU);
    }

    
    return(RM_OK);
}


static RM_STATUS class04DStartImage
(
    PHALHWINFO pHalHwInfo,
    PVIDEODECODERHALOBJECT pDecoderHalObj,
    U032 Buffer
)
{
    V032        stateSU, stateME;
    U032        xscaleincr;
    U032        yscaleincr;
    RM_STATUS   status;
    
    //DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class04DStartImage\r\n");

    //
    // Make sure the engine is configured correctly
    //
    if( pDecoderHalObj->lastImageStartLine == 99 ) {
      // program these up the first time through only
      FLD_WR_DRF_NUM(_PME, _656_CONFIG, _TASKA_ENABLE, 1);
      FLD_WR_DRF_DEF(_PME, _656_CONFIG, _TASKA_ONLY, _ENABLED);
      FLD_WR_DRF_DEF(_PME, _INTR_EN_0, _TASKA_NOTIFY, _ENABLED);
    }
    
    //
    // The hardware can only support dma transfers to framebuffer
    // addresses.  If this transfer is not completely contiguous,
    // we need to break the transfer up.
    //
    if (pDecoderHalObj->SetImage[Buffer].CtxDma->AddressSpace != ADDR_FBMEM)
        return (RM_ERROR);
    
    //
    // Calculate the scale factors given the in & out sizes
    //
    // only update scale weights if they change..
    if((pDecoderHalObj->lastWidthIn != pDecoderHalObj->SetImage[0].WidthIn ) 
         || (pDecoderHalObj->lastWidthOut != pDecoderHalObj->SetImage[0].WidthOut)) {
        status = class04DProgramWeights(pHalHwInfo,
                                        pDecoderHalObj->SetImage[0].WidthIn,
                                        pDecoderHalObj->SetImage[0].WidthOut);
      
      if (status)
          return (status);
    }  
                        
    //
    // Load up the buffer location and sizes
    //
    
    if (Buffer == 0)
    {
        REG_WR32(NV_PME_TASKA_BUFF0_START, pDecoderHalObj->SetImage[0].Offset
                                         + (pDecoderHalObj->SetImage[0].CtxDma->PteArray[0] & 0xFFFFF000)
                                         + pDecoderHalObj->SetImage[0].CtxDma->PteAdjust);

          REG_WR32(NV_PME_TASKA_BUFF0_PITCH, pDecoderHalObj->SetImage[0].Pitch);
          REG_WR32(NV_PME_TASKA_BUFF0_LENGTH, pDecoderHalObj->SetImage[0].Pitch * pDecoderHalObj->SetImage[0].HeightOut);
        
    }
    else
    {
        REG_WR32(NV_PME_TASKA_BUFF1_START, pDecoderHalObj->SetImage[1].Offset
                                         + (pDecoderHalObj->SetImage[1].CtxDma->PteArray[0] & 0xFFFFF000)
                                         + pDecoderHalObj->SetImage[1].CtxDma->PteAdjust);

          REG_WR32(NV_PME_TASKA_BUFF1_PITCH, pDecoderHalObj->SetImage[1].Pitch);
          REG_WR32(NV_PME_TASKA_BUFF1_LENGTH, pDecoderHalObj->SetImage[1].Pitch * pDecoderHalObj->SetImage[1].HeightOut);
    } 
    
    //
    // turn on scaling
    //
    if((pDecoderHalObj->lastHeightIn != pDecoderHalObj->SetImage[0].HeightIn ) || (pDecoderHalObj->lastHeightOut != pDecoderHalObj->SetImage[0].HeightOut)) {
    
      yscaleincr = ((pDecoderHalObj->SetImage[0].HeightOut << Y_SCALE_INCR_PRECISION) - 1) / 
                 (pDecoderHalObj->SetImage[0].HeightIn - 1);
      REG_WR_DRF_NUM(_PME, _TASKA_Y_SCALE, _INCR, yscaleincr);
      // REG_WR_DRF_NUM(_PME, _TASKA_Y_SCALE, _INCR, (1 << 10));
    }
    if((pDecoderHalObj->lastWidthIn != pDecoderHalObj->SetImage[0].WidthIn ) || (pDecoderHalObj->lastWidthOut != pDecoderHalObj->SetImage[0].WidthOut)) {
       
      xscaleincr = (65536 );
      if((pDecoderHalObj->SetImage[0].WidthOut != 0) && (pDecoderHalObj->SetImage[0].WidthIn != 0)) {
        // protection from  zero case
        xscaleincr = (65536 * pDecoderHalObj->SetImage[0].WidthIn) / pDecoderHalObj->SetImage[0].WidthOut;
      } 
      
      REG_WR_DRF_NUM(_PME, _TASKA_X_SCALE, _INCR, xscaleincr);
      FLD_WR_DRF_DEF(_PME, _TASKA_X_SCALE, _FILTER, _ENABLE);
    }

    //
    // program data width
    //

    if ( pDecoderHalObj->ImageDataWidth != (pDecoderHalObj->lastImageConfig & 0xFF) ) {
        if ( pDecoderHalObj->ImageDataWidth == 8 ) {
            FLD_WR_DRF_DEF(_PME, _656_CONFIG, _VIDEO, _8);
        }
        else if ( pDecoderHalObj->ImageDataWidth == 16 ) {
            FLD_WR_DRF_DEF(_PME, _656_CONFIG, _VIDEO, _16);
        }
        pDecoderHalObj->lastImageConfig = (pDecoderHalObj->lastImageConfig & 0xFFFFFF00) | pDecoderHalObj->ImageDataWidth;
    }

    //
    // program null data checking
    //

    if ( pDecoderHalObj->NullData != ((pDecoderHalObj->lastImageConfig >> 12) & 0x3) ) {
        if ( pDecoderHalObj->NullData == NV04D_SET_IMAGE_CONFIG_NULL_DATA_DISABLED ) {
            FLD_WR_DRF_DEF(_PME, _NULL_DATA, _COMPARE, _DISABLED);
            FLD_WR_DRF_DEF(_PME, _NULL_DATA, _LINE_DETECT, _DISABLED);
        }
        else if ( pDecoderHalObj->NullData == NV04D_SET_IMAGE_CONFIG_NULL_DATA_BYTE_ENABLED ) {
            FLD_WR_DRF_DEF(_PME, _NULL_DATA, _COMPARE, _ENABLED);
            FLD_WR_DRF_DEF(_PME, _NULL_DATA, _LINE_DETECT, _DISABLED);
            REG_WR_DRF_NUM(_PME, _NULL_DATA, _BYTE, pDecoderHalObj->ImageDataWidth);
        }
        else if ( pDecoderHalObj->NullData == NV04D_SET_IMAGE_CONFIG_NULL_DATA_LINE_ENABLED ) {
            FLD_WR_DRF_DEF(_PME, _NULL_DATA, _COMPARE, _ENABLED);
            FLD_WR_DRF_DEF(_PME, _NULL_DATA, _LINE_DETECT, _ENABLED);
            REG_WR_DRF_NUM(_PME, _NULL_DATA, _BYTE, pDecoderHalObj->ImageDataWidth);
        }
        pDecoderHalObj->lastImageConfig = (pDecoderHalObj->lastImageConfig & 0xFF) |
                                    ((pDecoderHalObj->Task & 0x1) << 8) |
                                    ((pDecoderHalObj->NullData & 0x3) << 12) |
                                    ((pDecoderHalObj->NullValue & 0x16) << 16);
    }

    //
    // Program the image startline
    //
    if(pDecoderHalObj->lastImageStartLine != pDecoderHalObj->ImageStartLine ) {
      REG_WR_DRF_NUM(_PME, _TASKA_Y_CROP, _STARTLINE, pDecoderHalObj->ImageStartLine);
    }

    //
    // program maximum line length
    //
    if(pDecoderHalObj->lastWidthIn != pDecoderHalObj->SetImage[0].WidthIn ) {
      if( pDecoderHalObj->SetImage[Buffer].WidthOut == 0 ) {
        // write the smallest possible value of line length that is still valid
        REG_WR32(NV_PME_TASKA_LINE_LENGTH, 0x4);
      } else {
        REG_WR32(NV_PME_TASKA_LINE_LENGTH, 2 * pDecoderHalObj->SetImage[0].WidthIn);
      }
    }
    //
    // Start the transfer
    //
    stateSU = REG_RD32(NV_PME_TASKA_SU_STATE);
    stateME = REG_RD32(NV_PME_TASKA_ME_STATE);

    if (Buffer == 0)
    {
        //
        // Choose the capture field (progressive == even)
        //
        if ((pDecoderHalObj->SetImage[0].Field == NV04D_SET_IMAGE_FORMAT_FIELD_PROGRESSIVE)
         || (pDecoderHalObj->SetImage[0].Field == NV04D_SET_IMAGE_FORMAT_FIELD_EVEN_FIELD))
            stateSU |= 1 << SF_SHIFT(NV_PME_TASKA_SU_STATE_BUFF0_FIELD);
        else            
            stateSU &= ~(1 << SF_SHIFT(NV_PME_TASKA_SU_STATE_BUFF0_FIELD));
            
        stateSU ^= 1 << SF_SHIFT(NV_PME_TASKA_SU_STATE_BUFF0_IN_USE);
        REG_WR32(NV_PME_TASKA_SU_STATE, stateSU);
    } 
    else // Buffer == 1
    {
        //
        // Choose the capture field (progressive == even)
        //
        if ((pDecoderHalObj->SetImage[1].Field == NV04D_SET_IMAGE_FORMAT_FIELD_PROGRESSIVE)
         || (pDecoderHalObj->SetImage[1].Field == NV04D_SET_IMAGE_FORMAT_FIELD_EVEN_FIELD))
            stateSU |= 1 << SF_SHIFT(NV_PME_TASKA_SU_STATE_BUFF1_FIELD);
        else            
            stateSU &= ~(1 << SF_SHIFT(NV_PME_TASKA_SU_STATE_BUFF1_FIELD));
    
        stateSU ^= 1 << SF_SHIFT(NV_PME_TASKA_SU_STATE_BUFF1_IN_USE);
        REG_WR32(NV_PME_TASKA_SU_STATE, stateSU);
    }
     
    pDecoderHalObj->lastWidthIn = pDecoderHalObj->SetImage[Buffer].WidthIn;
    pDecoderHalObj->lastWidthOut = pDecoderHalObj->SetImage[Buffer].WidthOut;
    pDecoderHalObj->lastHeightIn = pDecoderHalObj->SetImage[Buffer].HeightIn;
    pDecoderHalObj->lastHeightOut = pDecoderHalObj->SetImage[Buffer].HeightOut;
    pDecoderHalObj->lastImageStartLine = pDecoderHalObj->ImageStartLine;
    
    //DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class04DStartImage-end\r\n");

    return(RM_OK);
}

//---------------------------------------------------------------------------
//
//  Class method routines.
//
//---------------------------------------------------------------------------

RM_STATUS class04DMethod_NV20(PMPMETHODARG_000 pMpMethodArg)
{
    PVIDEODECODERHALOBJECT pDecoderHalObj;
    PHALHWINFO pHalHwInfo;
    U032 offset, data;
    U032 buffNum;

    pDecoderHalObj = (PVIDEODECODERHALOBJECT)pMpMethodArg->pHalObjInfo;
    offset = pMpMethodArg->offset;
    data = pMpMethodArg->data;
    pHalHwInfo = pMpMethodArg->pHalHwInfo;

    switch (offset)
    {
        case NV04D_STOP_TRANSFER_VBI:
            //
            // Disable interrupts
            // 
            FLD_WR_DRF_DEF(_PME, _INTR_EN_0, _ANC_NOTIFY, _DISABLED);

            //
            // Stop the current vbi data transfer immediately
            //
            // XXX Is this enough?
            //
            FLD_WR_DRF_DEF(_PME, _656_CONFIG, _ANC_MODE, _DISABLED);

            //
            // return things to a known state...
            // even though a buffer may STILL be running
            //
            REG_WR32(NV_PME_ANC_ME_STATE, 0);
            REG_WR32(NV_PME_ANC_SU_STATE, 0);
            REG_WR32(NV_PME_ANC_RM_STATE, 0);
            break;
        case NV04D_STOP_TRANSFER_IMAGE:
            //
            // Disable interrupts
            // 
            FLD_WR_DRF_DEF(_PME, _INTR_EN_0, _TASKA_NOTIFY, _DISABLED);
            FLD_WR_DRF_DEF(_PME, _INTR_EN_0, _TASKB_NOTIFY, _DISABLED);

            //
            // Stop the current image data transfer immediately.
            //
            // XXX Is this enough?
            //
            FLD_WR_DRF_NUM(_PME, _656_CONFIG, _TASKA_ENABLE, 0);
            FLD_WR_DRF_DEF(_PME, _656_CONFIG, _TASKA_ONLY, _DISABLED);
	 
            //
            // Return things to a known state...
            // even though a buffer may STILL be running
            //
            REG_WR32(NV_PME_TASKA_ME_STATE, 0);
            REG_WR32(NV_PME_TASKA_SU_STATE, 0);
            REG_WR32(NV_PME_TASKA_RM_STATE, 0);

            //
            // Reset the checks.
            //
            pDecoderHalObj->lastWidthIn               = 0;
            pDecoderHalObj->lastWidthOut              = 0;
            pDecoderHalObj->lastHeightIn              = 0;
            pDecoderHalObj->lastHeightOut             = 0;
            //
            // this is used to indicate the first time around...
            // 99 with likely never be used at a startline value since it
            // should be something like 0 - 21  
            //
            pDecoderHalObj->lastImageStartLine        = 99;
            break;
        case NV04D_SET_IMAGE_CONFIG:
        {
            U032 tmp;

            //
            // Make sure all the fields are legal before saving them.
            //
            tmp = data & 0xFF;
            if ( (tmp == 8) || (tmp == 16) ) {
                pDecoderHalObj->ImageDataWidth = tmp;
            }
            else {
                return NV04D_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT;
            }

            tmp = (data >> 8) & 0x1;
            // only support TASK A for now
            if ( tmp == NV04D_SET_IMAGE_CONFIG_TASK_A ) {
                pDecoderHalObj->Task = tmp;
            }
            else {
                return NV04D_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT;
            }

            tmp = (data >> 12) & 0x3;
            if ( (tmp == NV04D_SET_IMAGE_CONFIG_NULL_DATA_DISABLED) ||
                 (tmp == NV04D_SET_IMAGE_CONFIG_NULL_DATA_BYTE_ENABLED) ||
                 (tmp == NV04D_SET_IMAGE_CONFIG_NULL_DATA_LINE_ENABLED) ) {
                pDecoderHalObj->NullData = tmp;
            }
            else {
                return NV04D_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT;
            }
            pDecoderHalObj->NullValue = (data >> 16) & 0xFFFF;
            break;
        }
        case NV04D_SET_IMAGE_START_LINE:
            //
            // Load the local copy, but don't hit the hardware
            // until we actually need it.
            //
            if (data == 0)  data = 13;
            pDecoderHalObj->ImageStartLine = data;
            break;
        case NV04D_SET_VBI_SIZE(0):
        case NV04D_SET_VBI_SIZE(1):
            buffNum = (offset == NV04D_SET_VBI_SIZE(0)) ? 0 : 1;
            pDecoderHalObj->SetVbi[buffNum].FirstLine = (data & 0xFFFF);
            pDecoderHalObj->SetVbi[buffNum].Height    = ((data >> 16) & 0xFFFF);
            break;
        case NV04D_SET_VBI_OFFSET(0):
        case NV04D_SET_VBI_OFFSET(1):
            buffNum = (offset == NV04D_SET_VBI_OFFSET(0)) ? 0 : 1;
            pDecoderHalObj->SetVbi[buffNum].Offset = data;
            break;
        case NV04D_SET_VBI_FORMAT(0):
        case NV04D_SET_VBI_FORMAT(1):
            buffNum = (offset == NV04D_SET_VBI_FORMAT(0)) ? 0 : 1;
            pDecoderHalObj->SetVbi[buffNum].Pitch  = (data & 0xFFFF);
            pDecoderHalObj->SetVbi[buffNum].Field  = ((data >> 16) & 0xFF);
            pDecoderHalObj->SetVbi[buffNum].Notify = ((data >> 24) & 0xFF);
            //
            // Kick it off!!
            //
            class04DStartVbi(pHalHwInfo, pDecoderHalObj, buffNum);
            break;
        case NV04D_GET_VBI_OFFSET_NOTIFY(0):
        case NV04D_GET_VBI_OFFSET_NOTIFY(1):
            //
            // The scan position is the same for both buffers.
            //
            buffNum = (offset == NV04D_GET_VBI_OFFSET_NOTIFY(0)) ? 0 : 1;
            pDecoderHalObj->SetVbi[buffNum].GetOffsetData = REG_RD_DRF(_PME, _ANC_CURRENT, _POS);
            break;
        case NV04D_SET_IMAGE_SIZE_IN(0):
        case NV04D_SET_IMAGE_SIZE_IN(1):
            buffNum = (offset == NV04D_SET_IMAGE_SIZE_IN(0)) ? 0 : 1;
            pDecoderHalObj->SetImage[buffNum].WidthIn  = (data & 0xFFFF);
            pDecoderHalObj->SetImage[buffNum].HeightIn = ((data >> 16) & 0xFFFF);
            break;
        case NV04D_SET_IMAGE_SIZE_OUT(0):
        case NV04D_SET_IMAGE_SIZE_OUT(1):
            buffNum = (offset == NV04D_SET_IMAGE_SIZE_OUT(0)) ? 0 : 1;
            pDecoderHalObj->SetImage[buffNum].WidthOut  = (data & 0xFFFF);
            pDecoderHalObj->SetImage[buffNum].HeightOut = ((data >> 16) & 0xFFFF);
            //
            // Zero here is used to indicate we want a notify that a
            // field went by but no update.
            //
            if (pDecoderHalObj->SetImage[buffNum].HeightOut == 0)
                pDecoderHalObj->SetImage[buffNum].HeightOut = 1;
            break;
        case NV04D_SET_IMAGE_OFFSET(0):
        case NV04D_SET_IMAGE_OFFSET(1):
            buffNum = (offset == NV04D_SET_IMAGE_OFFSET(0)) ? 0 : 1;
            pDecoderHalObj->SetImage[buffNum].Offset = data;
            break;
        case NV04D_SET_IMAGE_FORMAT(0):
        case NV04D_SET_IMAGE_FORMAT(1):
            buffNum = (offset == NV04D_SET_IMAGE_FORMAT(0)) ? 0 : 1;
            pDecoderHalObj->SetImage[buffNum].Pitch  = (data & 0xFFFF);
            pDecoderHalObj->SetImage[buffNum].Field  = ((data >> 16) & 0xFF);
            pDecoderHalObj->SetImage[buffNum].Notify = ((data >> 24) & 0xFF);
            
            //
            // Kick it off
            //
            class04DStartImage(pHalHwInfo, pDecoderHalObj, buffNum);
            break;
        case NV04D_GET_IMAGE_OFFSET_NOTIFY(0):
        case NV04D_GET_IMAGE_OFFSET_NOTIFY(1):
            //
            // The scan position is the same for both buffers.
            //
            buffNum = (offset == NV04D_GET_IMAGE_OFFSET_NOTIFY(0)) ? 0 : 1;
            pDecoderHalObj->SetImage[buffNum].GetOffsetData = REG_RD_DRF(_PME, _TASK_CURRENT, _POS);
            break;
        default:
            // HAL doesn't have anything to do for this particular method
            ;
    }
    return NV04F_NOTIFICATION_STATUS_DONE_SUCCESS;
}

//---------------------------------------------------------------------------
//
//  Exception handling routines
//
//---------------------------------------------------------------------------

static VOID GetVbiEvents
(
    PHALHWINFO pHalHwInfo,
    PVIDEODECODERHALOBJECT pDecoderHalObj,
    U032 *pEventsPending
)
{
    U032 stateRM, stateSU, stateME, stateMESU, stateMERM;
    U032 eventsPending = 0;

    REG_WR32(NV_PME_INTR_0, DRF_DEF(_PME, _INTR_0, _ANC_NOTIFY, _RESET));

    //
    // Grab the current states
    //
    stateME = REG_RD32(NV_PME_ANC_ME_STATE);
    stateSU = REG_RD32(NV_PME_ANC_SU_STATE);
    stateRM = REG_RD32(NV_PME_ANC_RM_STATE);
    stateMESU = stateME ^ stateSU;
    stateMERM = stateME ^ stateRM;

    // buffer 0
    if (*pEventsPending & CLASS04D_VBI_EVENT(0))
    {
        if (!(stateMESU & 0x00010000))
        {
            if (stateMERM & 0x00000001)
            {
                eventsPending |= CLASS04D_VBI_EVENT(0);
            }
        }
    }

    // buffer 1
    if (*pEventsPending & CLASS04D_VBI_EVENT(1))
    {
        if (!(stateMESU & 0x00100000))
        {
            if (stateMERM & 0x00000010)
            {
                eventsPending |= CLASS04D_VBI_EVENT(1);
            }
        }
    }

    *pEventsPending &= ~(CLASS04D_VBI_EVENT(0)|CLASS04D_VBI_EVENT(1));
    *pEventsPending |= eventsPending;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: Leaving GetVbiEvents\n");

    return;
}

static VOID GetImageEvents
(
    PHALHWINFO pHalHwInfo,
    PVIDEODECODERHALOBJECT pDecoderHalObj,
    U032 *pEventsPending
)
{
    U032 stateRM, stateSU, stateME, stateMESU, stateMERM;
    U032 eventsPending = 0;
    
    // clear the interrupt, in case another shows up!
    REG_WR32(NV_PME_INTR_0, DRF_DEF(_PME, _INTR_0, _TASKA_NOTIFY, _RESET));
	
    //
    // Grab the current states
    //
    stateME = REG_RD32(NV_PME_TASKA_ME_STATE);
    stateSU = REG_RD32(NV_PME_TASKA_SU_STATE);
    stateRM = REG_RD32(NV_PME_TASKA_RM_STATE);
    stateMESU = stateME ^ stateSU;
    stateMERM = stateME ^ stateRM;

    // buffer 0

    if (*pEventsPending & CLASS04D_IMAGE_EVENT(0))
    {
        pDecoderHalObj->SetImage[0].Info32 = 0;
        if (!(stateMESU & 0x00010000))
        {
            if (stateMERM & 0x00000001)
            {
                eventsPending |= CLASS04D_IMAGE_EVENT(0);
                //
                // Used for callback support
                //
                pHalHwInfo->pMpHalInfo->ImageBufferNumber = REG_RD32( NV_PME_TASKA_BUFF0_START );
                pDecoderHalObj->SetImage[0].Info32 =
                    (REG_RD_DRF(_PME, _TASKA_ME_STATE, _BUFF0_ERROR_CODE) << 8 |
                    REG_RD_DRF(_PME, _TASKA_ME_STATE, _RP_FLAGS_BUFF0));
            }
        }
    }

    // buffer 1
    if (*pEventsPending & CLASS04D_IMAGE_EVENT(1))
    {
        pDecoderHalObj->SetImage[1].Info32 = 0;
        if (!(stateMESU & 0x00100000))
        {
            if (stateMERM & 0x00000010)
            {
                eventsPending |= CLASS04D_IMAGE_EVENT(1);
                //
                // Used for callback support
                //
                pHalHwInfo->pMpHalInfo->ImageBufferNumber = REG_RD32( NV_PME_TASKA_BUFF1_START );
                pDecoderHalObj->SetImage[1].Info32 =
                    (REG_RD_DRF(_PME, _TASKA_ME_STATE, _BUFF1_ERROR_CODE) << 8 |
                    REG_RD_DRF(_PME, _TASKA_ME_STATE, _RP_FLAGS_BUFF1));
            }
        }
    }	

    *pEventsPending &= ~(CLASS04D_IMAGE_EVENT(0)|CLASS04D_IMAGE_EVENT(1));
    *pEventsPending |= eventsPending;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: Leaving GetImageEvents\n");

    return;
}

//
// class04DGetEventStatus
//
// This call returns the status of the specified MP event(s).
//
RM_STATUS class04DGetEventStatus_NV20
(
    PMPGETEVENTSTATUSARG_000 pMpGetEventStatusArg
)
{
    PHALHWINFO pHalHwInfo;
    PVIDEODECODERHALOBJECT pDecoderHalObj;
    U032 intr0;
    U032 eventsPending;

    pHalHwInfo = pMpGetEventStatusArg->pHalHwInfo;

    //
    // Get current interrupt status and save off in exception frame.
    //
    intr0 = REG_RD32(NV_PME_INTR_0);
    pMpGetEventStatusArg->intrStatus = intr0;

    //
    // Check for pending VBI/IMAGE events.
    //
    if (!(intr0 & (DRF_DEF(_PME, _INTR_0, _ANC_NOTIFY, _PENDING) |
                   DRF_DEF(_PME, _INTR_0, _TASKA_NOTIFY, _PENDING))))
    {
        pMpGetEventStatusArg->events = 0;
        return RM_OK;
    }

    //
    // No current decoder object, so treat exception as spurious.
    //
    if (pMpGetEventStatusArg->pHalObjInfo == NULL)
    {
        REG_WR32(NV_PME_INTR_0, DRF_DEF(_PME, _INTR_0, _ANC_NOTIFY, _RESET));
        REG_WR32(NV_PME_INTR_0, DRF_DEF(_PME, _INTR_0, _TASKA_NOTIFY, _RESET));
        pMpGetEventStatusArg->events = 0;
        return (RM_ERROR);
    }

    pDecoderHalObj = (PVIDEODECODERHALOBJECT)pMpGetEventStatusArg->pHalObjInfo;

    //
    // The caller specifies a bit field of events for which it wishes
    // to get status.  If an event of interest isn't pending, then it's
    // bit is cleared.
    //
    eventsPending = pMpGetEventStatusArg->events;
    if (eventsPending & (CLASS04D_VBI_EVENT(0)|CLASS04D_VBI_EVENT(1)))
    {
        if (intr0 & DRF_DEF(_PME, _INTR_0, _ANC_NOTIFY, _PENDING))
            GetVbiEvents(pHalHwInfo, pDecoderHalObj, &eventsPending);
        else
            eventsPending &= ~(CLASS04D_VBI_EVENT(0)|CLASS04D_VBI_EVENT(1));
    }
    if (eventsPending & (CLASS04D_IMAGE_EVENT(0)|CLASS04D_IMAGE_EVENT(1)))
    {
        if (intr0 & DRF_DEF(_PME, _INTR_0, _TASKA_NOTIFY, _PENDING))
            GetImageEvents(pHalHwInfo, pDecoderHalObj, &eventsPending);
        else
            eventsPending &= ~(CLASS04D_IMAGE_EVENT(0)|CLASS04D_IMAGE_EVENT(1));
    }

    //
    // Return set of events that are actually pending.
    //
    pMpGetEventStatusArg->events = eventsPending;

    return RM_OK;
}

//
// class04DServiceEvent
//
// Finish servicing specified event (i.e. reset buffer status).
//
RM_STATUS class04DServiceEvent_NV20
(
    PMPSERVICEEVENTARG_000 pMpServiceEventArg
)
{
    PHALHWINFO pHalHwInfo;
    U032 event, stateRM;
    RM_STATUS status = RM_OK;

    pHalHwInfo = pMpServiceEventArg->pHalHwInfo;

    //
    // The values were setup by the GetEventStatus interface.
    //
    event = pMpServiceEventArg->event;

    //
    // The eventsPending value should only specify a single
    // VBI *or* image buffer.
    //
    if (event == CLASS04D_VBI_EVENT(0))
    {
        stateRM = REG_RD32(NV_PME_ANC_RM_STATE);
        stateRM ^= 1 << SF_SHIFT(NV_PME_ANC_RM_STATE_BUFF0_INTR_NOTIFY);
        REG_WR32(NV_PME_ANC_RM_STATE, stateRM);
    }
    else if (event == CLASS04D_VBI_EVENT(1))
    {
        stateRM = REG_RD32(NV_PME_ANC_RM_STATE);
        stateRM ^= 1 << SF_SHIFT(NV_PME_ANC_RM_STATE_BUFF1_INTR_NOTIFY);
        REG_WR32(NV_PME_ANC_RM_STATE, stateRM);
    }
    else if (event == CLASS04D_IMAGE_EVENT(0))
    {
        stateRM = REG_RD32(NV_PME_TASKA_RM_STATE);
        stateRM ^= 1 << SF_SHIFT(NV_PME_TASKA_RM_STATE_BUFF0_INTR_NOTIFY);
        REG_WR32(NV_PME_TASKA_RM_STATE, stateRM);
    }
    else if (event == CLASS04D_IMAGE_EVENT(1))
    {
        stateRM = REG_RD32(NV_PME_TASKA_RM_STATE);
        stateRM ^= 1 << SF_SHIFT(NV_PME_TASKA_RM_STATE_BUFF1_INTR_NOTIFY);
        REG_WR32(NV_PME_TASKA_RM_STATE, stateRM);
    }
    else
    {
        status = RM_ERR_BAD_ARGUMENT;
    }

    //
    // XXX (scottl):
    //
    // Shouldn't this be #ifdef DEBUG???
    //
    if (event == CLASS04D_VBI_EVENT(0) || event == CLASS04D_VBI_EVENT(1))
    {
        U032 error;
        // and out the rest of the bits
        error = REG_RD32(NV_PME_ANC_ME_STATE) & 0x7E00;
        if (error) {
            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: C04DNV20: errors detected in ME register\n");
        }
    }

    //
    // Return updated pending interrupt values.
    //
    pMpServiceEventArg->intrStatus = REG_RD32(NV_PME_INTR_0);
    
    return status;
}
