//#define VERBOSE    // VERBOSE doesn't work with multiproc!!
/***********************************************************************************/

#include "bldsetup.h"

#include <stdlib.h>
#include <math.h>
#ifndef __arm
#   include <memory.h>
#else
#   include <memory>
#endif
#include "xplatform.h"
#include "typedef.hpp"
#include "wmvdec_api.h"
#include "spatialpredictor_wmv.hpp"
#ifdef _WMV_TARGET_X86_
#include "cpudetect.h"
#endif // _WMV_TARGET_X86_

/****************************************************************************************
  Array declarations
****************************************************************************************/

//  Bidirectional prediction weights
U16_WMV aNullPredWts_WMV[] = {
    640, 640, 669, 480, 708, 354, 748, 257, 792, 198, 760, 143, 808, 101, 772,  72,
    480, 669, 537, 537, 598, 416, 661, 316, 719, 250, 707, 185, 768, 134, 745,  97,
    354, 708, 416, 598, 488, 488, 564, 388, 634, 317, 642, 241, 716, 179, 706, 132,
    257, 748, 316, 661, 388, 564, 469, 469, 543, 395, 571, 311, 655, 238, 660, 180,
    198, 792, 250, 719, 317, 634, 395, 543, 469, 469, 507, 380, 597, 299, 616, 231,
    161, 855, 206, 788, 266, 710, 340, 623, 411, 548, 455, 455, 548, 366, 576, 288,
    122, 972, 159, 914, 211, 842, 276, 758, 341, 682, 389, 584, 483, 483, 520, 390,
    110, 1172, 144, 1107, 193, 1028, 254, 932, 317, 846, 366, 731, 458, 611, 499, 499
};

/****************************************************************************************
  Constructor for CSpatialPredictor
****************************************************************************************/
#ifndef _XBOX
Bool_WMV t_SpatialPredictor::m_bIsMMX = FALSE_WMV;
#endif

t_SpatialPredictor *t_SpatialPredictorConstruct ()
{
    t_SpatialPredictor  *pSp = (t_SpatialPredictor *) wmvalloc (sizeof(t_SpatialPredictor));
    if (!pSp)
        goto ERROR_EXIT;

    pSp->m_pNeighbors = (U8_WMV *) (wmvalloc (16 * 4)); // align to 4 byte (U32_WMV) and cast
    if (!pSp->m_pNeighbors)
        goto ERROR_EXIT;

    pSp->m_pBuffer16 = (I16_WMV *) wmvalloc (24 * 2);
    if (!pSp->m_pBuffer16)
        goto ERROR_EXIT;

    // tune accesses
    pSp->m_pLeft = pSp->m_pNeighbors + 31;
    pSp->m_pTop = pSp->m_pNeighbors + 32;

    // Set up sum arrays
    pSp->m_pTopSum = (pSp->m_pSums = (U16_WMV *) wmvalloc (32 * 4)) + 4;  // extra space for MMX setupZero
    if (!pSp->m_pSums)
        goto ERROR_EXIT;
    pSp->m_pLeftSum = pSp->m_pTopSum + 12;

    DEBUG_HEAP_ADD(3, sizeof(t_SpatialPredictor) + (16 * 4) + (24 * 2) + (32 * 4));

#ifdef _WMV_TARGET_X86_
#ifndef _XBOX
    if (g_SupportMMX()) {
        pSp->m_bIsMMX = TRUE;  // this variable is used for IDCT mode only
//        m_pDeblock = (g_SupportCMOV()) ? deblock_MMX : deblock_MMX_noCMOV;
    }
#endif // _XBOX
#endif // _WMV_TARGET_X86_

    return pSp;

ERROR_EXIT:
    t_SpatialPredictorDestruct (pSp);
    return NULL_WMV;
}

/****************************************************************************************
  Destructor for CSpatialPredictor
****************************************************************************************/
Void_WMV t_SpatialPredictorDestruct (t_SpatialPredictor *pSp)
{
    if ( pSp != NULL ) {
        if ( pSp->m_pSums != NULL )
            wmvfree (pSp->m_pSums);
        if ( pSp->m_pBuffer16 != NULL )
            wmvfree (pSp->m_pBuffer16);
        if ( pSp->m_pNeighbors != NULL )
            wmvfree (pSp->m_pNeighbors);
        wmvfree (pSp);
    }
    DEBUG_HEAP_SUB(3, sizeof(t_SpatialPredictor) + (16 * 4) + (24 * 2) + (32 * 4));
}

/****************************************************************************************
  Set up neighbors
****************************************************************************************/
/** old code
Void_WMV CSpatialPredictor::setupZeroPredictor_C_fast (Void_WMV)
{
    I16_WMV      *pTop  = m_pBuffer16 + 12;
    I16_WMV      *pLeft = m_pBuffer16;
    U16_WMV     *pTsum = m_pTopSum;
    U16_WMV     *pLsum = m_pLeftSum;

    I64  S1[5], S3[5], *pS1, *pS3;
    pS1 = S1;
    *pS1 = 0;
    pS3 = S3;

    for (I8_WMV i = 0; i < 12; i++) {
        pTop[i]  = I16_WMV  (m_pTop[i]);
        pLeft[i] = I16_WMV  (m_pLeft[-i - 1]);
    }
    // facilitate (increment sl only for k < 8)
    pLeft[8] = pLeft[9] = pLeft[10] = pLeft[11] = 0;

    I32_WMV     *pTop32 = (I32_WMV *) pTop;
    I32_WMV     *pLeft32 = (I32_WMV *) pLeft;
    I32_WMV     *pS3_32 = (I32_WMV *) pS3;
    I32_WMV     *pS1_32 = (I32_WMV *) pS1;

    {
        I32_WMV     iMM1t, iMM1l, iMM7t, iMM7l, iMM5t, iMM5l;
        I32_WMV     iMM2t, iMM2l, iMM6t, iMM6l, iMM4t, iMM4l;

        iMM1t = (pTop32[0] << 4);  iMM1l = (pLeft32[0] << 4);
        iMM7t = iMM1t;
        iMM7l = iMM1l;
        pS1_32[2] = iMM1l;  pS1_32[3] = iMM1t;

        iMM1t >>= 1;  iMM1l >>= 1;
        iMM6t = (pTop32[1] << 4);  iMM6l = (pLeft32[1] << 4);
        pS1_32[4] = iMM1l += iMM6l;  pS1_32[5] = iMM1t += iMM6t;

        iMM1t >>= 1;  iMM1l >>= 1;
        iMM5t = (pTop32[2] << 4);  iMM5l = (pLeft32[2] << 4);
        pS1_32[6] = iMM1l += iMM5l;  pS1_32[7] = iMM1t += iMM5t;

        iMM1t >>= 1;  iMM1l >>= 1;
        iMM2t = iMM4t = (pTop32[3] << 4);  iMM2l = iMM4l = (pLeft32[3] << 4);
        pS1_32[8] = iMM1l += iMM4l;  pS1_32[9] = iMM1t += iMM4t;

        iMM2t >>= 1;  iMM2l >>= 1;
        pS3_32[4] = iMM5l += iMM2l;  pS3_32[5] = iMM5t += iMM2t;

        iMM5t >>= 1;  iMM5l >>= 1;
        pS3_32[2] = iMM6l += iMM5l;  pS3_32[3] = iMM6t += iMM5t;

        iMM6t >>= 1;  iMM6l >>= 1;
        pS3_32[0] = iMM7l += iMM6l;  pS3_32[1] = iMM7t += iMM6t;

        iMM1t = (pTop32[4] << 3);
        pS3_32[6] = iMM4l;  pS3_32[7] = iMM4t += iMM1t;
        iMM1t += iMM1t;

        iMM4t = (pTop32[5] << 3);
        pS3_32[8] = 0;  pS3_32[9] = iMM1t += iMM4t;
    }

    for (I32_WMV iB = 0; iB < 10; iB += 2) {
        I16_WMV   *pS3l = (I16_WMV  *)(pS3_32 + iB);
        I16_WMV   *pS3t = pS3l + 2;

        I16_WMV   *pS1l = (I16_WMV  *)(pS1_32 + iB);
        I16_WMV   *pS1t = pS1l + 2;

        pLsum[iB - 1] = pS1l[1] + (pS3l[1] >> 1) + (((pS1l[0] + pS3l[0]) * 181 + 128) >> 8);
        pLsum[iB] = pS3l[0] + (pS1l[0] >> 1) + (((pS1l[1] + pS3l[1]) * 181 + 128) >> 8);

        pTsum[iB - 1]  = pS1t[1] + (pS3t[1] >> 1) + (((pS1t[0] + pS3t[0]) * 181 + 128) >> 8);
        pTsum[iB]  = pS3t[0] + (pS1t[0] >> 1) + (((pS1t[1] + pS3t[1]) * 181 + 128) >> 8);

    }
}
**/


/****************************************************************************************
  Spatial prediction function for decoder
****************************************************************************************/
Bool_WMV bMin_Max_LE_2QP (I32_WMV a,I32_WMV b,I32_WMV c,I32_WMV d,I32_WMV e,I32_WMV f,I32_WMV g,I32_WMV h, I32_WMV i2Qp)
{
    I32_WMV min, max;
    if (a >= h){ 
        min = h; max = a;
    }
    else{
        min = a; max = h;
    }
    if (min > c) min  = c;
    else if (c > max) max = c;
    if (min > e) min  = e;
    else if (e > max) max = e;

////////////// Run Experiement to see if it benefit.
    if (max - min >= i2Qp) return FALSE_WMV;

    if (min > b) min  = b;
    else if (b > max) max = b;
    if (min > d) min  = d;
    else if (d > max) max = d;
    if (min > f) min  = f;
    else if (f > max) max = f;
    if (min > g) min  = g;
    else if (g > max) max = g;

    return (Bool_WMV) (max - min < i2Qp);
}


#if !defined( OPT_X8_ARM )&& !defined(_MIPS_ASM_X8_OPT_)

#define phi1(a) ((U32_WMV)(a+iThr1) <= uThr2)

static Void_WMV FilterHorzEdgeX8 (PixelC* ppxlcCenter, I32_WMV iPixelDistance, I32_WMV iStepSize)
{
    const I32_WMV iThr1 = ((iStepSize + 10) >> 3);
    const U32_WMV uThr2 = iThr1<<1;
    
    for (I32_WMV i = 0; i < 8; ++i) {
        I32_WMV  v1, v2, v3, v4, v5, v6, v7, v8;
        PixelC* pVtmpR = ppxlcCenter;
        PixelC* pVtmpL = ppxlcCenter - iPixelDistance;

        v4 = *pVtmpL;
        v5 = *pVtmpR;
        pVtmpL -= iPixelDistance;
        pVtmpR += iPixelDistance;
        v3 = *pVtmpL;
        v6 = *pVtmpR;
        pVtmpL -= iPixelDistance;
        pVtmpR += iPixelDistance;
        v2 = *pVtmpL;
        v7 = *pVtmpR;
        pVtmpL -= iPixelDistance;
        pVtmpR += iPixelDistance;
        v1 = *pVtmpL;
        v8 = *pVtmpR;

        I32_WMV eq_cnt = phi1(v1 - v2) + phi1(v2 - v3) + phi1(v3 - v4) + phi1(v4 - v5);
        if (eq_cnt != 0)
            eq_cnt += phi1(pVtmpL[-iPixelDistance] - v1) + phi1(v5 - v6) + phi1(v6 - v7) + phi1(v7 - v8) + phi1(v8 - pVtmpR[iPixelDistance]);

        Bool_WMV bStrongFilter;
        if (eq_cnt >= 6) {
            bStrongFilter = bMin_Max_LE_2QP (v1, v2, v3, v4, v5, v6, v7, v8, iStepSize*2);        
        } else {
            bStrongFilter = FALSE;
        }
        
        if (bStrongFilter) {
            I32_WMV  v2plus7 = v2 + v7;            
            *(ppxlcCenter - 2*iPixelDistance) = (3 * (v2 + v3) + v2plus7 + 4) >> 3;                                                        
            *(ppxlcCenter + iPixelDistance) = (3 * (v7 + v6) + v2plus7 + 4) >> 3;
            v2plus7 <<= 1;
            *(ppxlcCenter - iPixelDistance) = (v2 + 3 * v4 + v2plus7 + 4) >> 3;                                                        
            *(ppxlcCenter) = (v7 + 3 * v5 + v2plus7 + 4) >> 3;                                                        
        }
        else {

            I32_WMV v4_v5 = v4 - v5;
            I32_WMV a30 = (2*(v3-v6) - 5*v4_v5 + 4) >> 3;
            I32_WMV absA30 = abs(a30);
            if (absA30 < iStepSize) {
                
                v2 -= v3;
                v6 -= v7;
                I32_WMV a31 = (2 * (v1-v4) - 5 * v2 + 4) >> 3;                                 
                I32_WMV a32 = (2 * (v5-v8) - 5 * v6 + 4) >> 3;
                
                I32_WMV iMina31_a32 = min(abs(a31),abs(a32)); 
                absA30 -= iMina31_a32;
                
                if ((absA30 > 0) && ((v4_v5 ^ a30) < 0)){

                    I32_WMV iSign = v4_v5 >> 31;
                    v4_v5 = abs(v4_v5) >> 1;

                    absA30 *= 5;
                    absA30 >>= 3;

                    if (absA30 > v4_v5)
                        absA30 = v4_v5;

                    absA30 ^= iSign;
                    absA30 -= iSign;

                    *(ppxlcCenter - iPixelDistance) = v4 - absA30;                                                                                         
                    *(ppxlcCenter) = v5 + absA30;
                }
            }        
        }        
        ppxlcCenter++;
    }
}

static Void_WMV FilterVertEdgeX8 (PixelC* ppxlcCenter, I32_WMV iPixelIncrement, I32_WMV iStepSize)
{
    const I32_WMV iThr1 = ((iStepSize + 10) >> 3);// + 1;
    const U32_WMV uThr2 = iThr1<<1;
    PixelC* pVh = ppxlcCenter - 5;
                
    for (I32_WMV i = 0; i < 8; ++i) {
        
        //I32_WMV v0 = pVh[0];                                                                    
        I32_WMV v1 = pVh[1];                                                                    
        I32_WMV v2 = pVh[2];                                                                    
        I32_WMV v3 = pVh[3];                                                                    
        I32_WMV v4 = pVh[4];                                                                    
        I32_WMV v5 = pVh[5];                                                                    
        I32_WMV v6 = pVh[6];                                                                    
        I32_WMV v7 = pVh[7];                                                                    
        I32_WMV v8 = pVh[8];                                                                    
        //I32_WMV v9 = pVh[9];                                                                    
                
        I32_WMV eq_cnt = phi1(v1 - v2) + phi1(v2 - v3) + phi1(v3 - v4) + phi1(v4 - v5);        
        if (eq_cnt != 0)
            eq_cnt += phi1(pVh[0] - v1) + phi1(v5 - v6) + phi1(v6 - v7) + phi1(v7 - v8) + phi1(v8 - pVh[9]);

        Bool_WMV bStrongFilter;
        if (eq_cnt >= 6) {
            bStrongFilter = bMin_Max_LE_2QP (v1, v2, v3, v4, v5, v6, v7, v8, iStepSize*2); 
            
        } else {
            bStrongFilter = FALSE;
        }
        
        if (bStrongFilter) {            
            I32_WMV  v2plus7 = v2 + v7;
            pVh[3] = (3 * (v2 + v3) + v2plus7 + 4) >> 3;                                                        
            pVh[6] = (3 * (v7 + v6) + v2plus7 + 4) >> 3;
            v2plus7 <<= 1;
            pVh[4] = (v2 + 3 * v4 + v2plus7 + 4) >> 3;                                                        
            pVh[5] = (v7 + 3 * v5 + v2plus7 + 4) >> 3;       
        }
        else {
            I32_WMV v4_v5 = v4 - v5;
            I32_WMV a30 = (2*(v3-v6) - 5*v4_v5 + 4) >> 3;
            I32_WMV absA30 = abs(a30);
            if (absA30 < iStepSize) {
                
                v2 -= v3;
                v6 -= v7;
                I32_WMV a31 = (2 * (v1-v4) - 5 * v2 + 4) >> 3;                                 
                I32_WMV a32 = (2 * (v5-v8) - 5 * v6 + 4) >> 3;                                 

                I32_WMV iMina31_a32 = min(abs(a31),abs(a32)); 
                absA30 -= iMina31_a32;
                
                if ((absA30 > 0) && ((v4_v5 ^ a30) < 0)){

                    I32_WMV iSign = v4_v5 >> 31;
                    v4_v5 = abs(v4_v5) >> 1;

                    absA30 *= 5;
                    absA30 >>= 3;

                    if (absA30 > v4_v5)
                        absA30 = v4_v5;

                    absA30 ^= iSign;
                    absA30 -= iSign;

                    pVh[4] = v4 - absA30;                                                                                         
                    pVh[5] = v5 + absA30;
                }
            }   
        }
        
        pVh += iPixelIncrement;
    }
}

#else   // OPT_X8_ARM

// defined in ASM
extern "C" Void_WMV FilterHorzEdgeX8 (PixelC* ppxlcCenter, I32_WMV iPixelDistance, I32_WMV iStepSize);
extern "C" Void_WMV FilterVertEdgeX8 (PixelC* ppxlcCenter, I32_WMV iPixelIncrement, I32_WMV iStepSize);

#   if defined(DEBUG) 
        // a easy breakpoint mechanism from within asm
        extern "C" void WMVAsmBreak( int r0, int r1 ) {
            volatile int i = r0 != r1;
        }
#   endif

#endif  // OPT_X8_ARM

/***************************************************************************
  Deblocking function
***************************************************************************/
Void_WMV X8Deblock(U8_WMV *pCenter, I32_WMV iStride, I32_WMV iStepSize,
                   I32_WMV blockX, I32_WMV blockY, Bool_WMV bResidual, I32_WMV iOrient)
{
    Bool_WMV  bFlag;

    FUNCTION_PROFILE_DECL_START(fp,DECODEIX8LOOPFILTER_PROFILE);

    bFlag = (blockY > 0) && (bResidual || (iOrient != 0 && iOrient != 4));
	if (bFlag)
        FilterHorzEdgeX8(pCenter, iStride, iStepSize);

    bFlag = (blockX > 0) && (bResidual || (iOrient != 0 && iOrient != 8));
    if (bFlag)
        FilterVertEdgeX8(pCenter, iStride, iStepSize);

    FUNCTION_PROFILE_STOP(&fp);
}

/***************************************************************************
  Context class global functions
***************************************************************************/
static U8_WMV fnMin(U8_WMV v1, U8_WMV v2, U8_WMV v3)
{
    return (v1<v2)?((v1<v3)?v1:v3):((v2<v3)?v2:v3);
}

/***************************************************************************
  Context class constructor and destructor
***************************************************************************/
t_ContextWMV *t_ContextWMVConstruct (I32_WMV iCol, I32_WMV iRow)
{
    t_ContextWMV *pContext = (t_ContextWMV *) wmvalloc (sizeof(t_ContextWMV));
    if ( pContext == NULL_WMV )
        return NULL_WMV;
    pContext->m_iRow = iRow;
    pContext->m_iCol = iCol;

    pContext->m_pData = (U8_WMV *) wmvalloc (iRow * iCol * sizeof(U8_WMV));
    if (pContext->m_pData == NULL_WMV)
        return NULL_WMV;

    DEBUG_HEAP_ADD(DHEAP_LOCALHUFF, sizeof(t_ContextWMV) + (iRow * iCol * sizeof(U8_WMV)));

    pContext->m_iColGt2 = (pContext->m_iCol > 2) ? 2 : 1;
    return pContext;
}

Void_WMV t_ContextWMVDestruct (t_ContextWMV *pContext)
{
    if ( pContext != NULL ) {
        DEBUG_HEAP_SUB(DHEAP_LOCALHUFF, sizeof(t_ContextWMV) + (pContext->m_iRow * pContext->m_iCol * sizeof(U8_WMV)));
        if ( pContext->m_pData != NULL )
            wmvfree ((Void_WMV *)(pContext->m_pData));
        wmvfree ((Void_WMV *) pContext);
    }
}

/***************************************************************************
  Context class joint functions (only decoder side is safe)
***************************************************************************/
Void_WMV t_ContextGetDec (t_ContextWMV *pContext, I32_WMV iX, I32_WMV iY,
                          I32_WMV iStepSize, I32_WMV *iPredOrient, I32_WMV *iEstRun)
{
    U8_WMV  *pData1 = pContext->m_pData + ((iY - 1) & 1) * pContext->m_iCol;
    U8_WMV  *pData0 = pContext->m_pData + (iY & 1) * pContext->m_iCol;

// predict orientation
    if ((iX | iY) == 0) {
        *iPredOrient = 0;
    }
    else if (iX == 0) {  // left edge (vertical)
// check if top far right orientation is checked in (at least 1 MB wide)
        *iPredOrient = 1;
        // following check not required at decoder
        // iPredOrient = ((pData1[m_iColGt2] & 0x3) == 0x3) ? 0xff : 1;
    }
    else if (iY == 0) {  // top edge  (horizontal)
// no check needed
        *iPredOrient = 2;
    }
    else {
// check top far right
        I32_WMV  iXfr = iX+2;
        I32_WMV iT, iL;
        if (iXfr >= pContext->m_iCol)  iXfr = pContext->m_iCol - 1;
        // following check not required at decoder
        //if ((pData1[iXfr] & 0x3) == 0x3) {
        //    iPredOrient = 0xff;
        //    return;
        //}
// all the rules
        
        iT = pData1[iX] & 0x3;
        iL = pData0[iX - 1] & 0x3;

        if (iT==iL)
            *iPredOrient =  iT;
        else if (iL==2 && iT==0)
            *iPredOrient =  iL;
        else if (iL==0 && iT==1)
            *iPredOrient =  iT;
        else if (iL==2 && iT==1)
            *iPredOrient =  iL;
        else if (iL==1 && iT==2) {
            I32_WMV iTL = pData1[iX - 1] & 0x3;
            if (iTL == iL)            *iPredOrient =  iT;
            else {
                if (iStepSize > 12)   *iPredOrient =  iT;
                else {
                    if (iTL == iT)    *iPredOrient =  iL;
                    else              *iPredOrient =  iTL;
                }
            }
        }
        else
            *iPredOrient =  0;
    }

    {
        U8_WMV v1, v2, v3;
        if (iX == 0) {
            v2 = v1 = (iY == 0) ?
                /*m_uDefault*/ 16 : (pData1[0] >> 2);
        }
        else {
            v1 = (pData0[iX - 1] >> 2);
            v2 = (iY == 0)?
                v1 : (pData1[iX] >> 2);
        }
        v3 = ((iY & iX) == 0) ? v2 : (pData1[iX - 1] >> 2);

        *iEstRun = fnMin (v1, v2, v3);
    }
    return;
}

/***************************************************************************
  Context class joint put (only decoder side is safe)
***************************************************************************/
static I8_WMV orientRemap[]={0,0,0,0,1,0,0,0,2,0,0,0};

Void_WMV t_ContextPutDec (t_ContextWMV *pContext, I32_WMV iX, I32_WMV iY,
                          I32_WMV iOrientContext, I32_WMV iRunContext)
{
    iOrientContext = orientRemap[iOrientContext] & 0x3;
    pContext->m_pData[(iY & 1) * pContext->m_iCol + iX]
        = iOrientContext | (iRunContext << 2);
}

/***************************************************************************
  Orientation predictor (joint, for Chroma)
***************************************************************************/
I32_WMV  t_ContextGetChromaDec (t_ContextWMV *pContext, I32_WMV iX, I32_WMV iY)
{
    if ((iX > 1) && (iY > 1))
        return pContext->m_pData[iX - 1] & 0x3;
    else if (iY > 1)
        return 1;   // vertical predictor
    else
        return 2;   // horizontal predictor (also for origin block)
}

/***************************************************************************
  END
***************************************************************************/
