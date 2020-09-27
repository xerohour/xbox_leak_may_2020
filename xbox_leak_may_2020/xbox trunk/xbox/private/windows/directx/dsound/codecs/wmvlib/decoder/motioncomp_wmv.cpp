/*************************************************************************

Copyright (C) Microsoft Corporation, 1996 - 1999

Module Name:

	MB.hpp

Abstract:

	MacroBlock base class

Author:
	Bruce Lin (blin@microsoft.com) 25-Jan-1997
		Compute curr (Error + Pred) in MC routine.
	Bruce Lin (blin@microsoft.com) 23-Jan-1997
		Optimize MC routines by expanding inner loop
	Ming-Chieh Lee (mingcl@microsoft.com) 20-March-1996
	Bruce Lin (blin@microsoft.com) 20-March-1996
    Chuang Gu (chuanggu@microsoft.com) 2-Feb-2000

Revision History:

*************************************************************************/
#ifndef _EMB_WMV2_

#include "bldsetup.h"

#include "xplatform.h"
#include <limits.h>
#include "typedef.hpp"
#include "motioncomp_wmv.h"
#include "opcodes.h"
#include "wmvdec_api.h"
#include "wmvdec_member.h"

#define _USE_PORTABLE_C_

#ifdef UNDER_CE
#undef _FASTMC_
#endif

#if defined( _MIPS_)&&defined(MIPS_WMVIDEO)
#undef _USE_PORTABLE_C_
#define _USE_MIPS_ASM_
#endif


#if defined(_MIPS_)
extern "C" {
	Void_WMV __asm(char *,...);
}
#pragma intrinsic(__asm)
#endif


#include <stdlib.h>
#include <math.h>

#ifdef __MFC_
#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW
#endif // __MFC_

#ifdef UNDER_CE
#define __UNIXVIDEO__
#endif

#define maskStaturation 0xFFFFFF00

#ifndef __MACVIDEO__
#define packFourPixels(x0,x1,x2,x3) ((U32_WMV)x3<<24)|((U32_WMV)x2<<16)|((U32_WMV)x1<<8)|(U32_WMV)x0
#else
#define packFourPixels(x0,x1,x2,x3) ((U32_WMV)x0<<24)|((U32_WMV)x1<<16)|((U32_WMV)x2<<8)|(U32_WMV)x3
#endif
   
#ifdef BIG_ENDING 
#define packFourPixels(x0,x1,x2,x3) ((U32_WMV)x0<<24)|((U32_WMV)x1<<16)|((U32_WMV)x2<<8)|(U32_WMV)x3
#endif

I32_WMV g_iMotionCompDecEventCount_WMV = 0;


#ifdef _USE_PORTABLE_C_ 
Void_WMV g_MotionCompAndAddErrorRndCtrl (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* ppxlcCurrQMB,
    const Buffer __huge* ppxliErrorBuf, const U8_WMV* ppxlcRef,
    I32_WMV iWidthFrm, Bool_WMV bXEven, Bool_WMV bYEven,
    I32_WMV iMixedPelMV	
)
{
    const U8_WMV* rgiClapTab = pWMVDec->m_rgiClapTabDec; 

//#ifndef _CASIO_VIDEO_
//    assert (rgiClapTab != NULL_WMV);
//#endif
    FUNCTION_PROFILE_DECL_START(fpDecode,MOTIONCOMPADDERRORRNDCTRL_PROFILE);

    const PixelI32 *ppxliErrorQMB = ppxliErrorBuf->i32;

    I32_WMV iy;
    if (bYEven) {
        if (bXEven) {  //!bXSubPxl && !bYSubPxl
            for (iy = 0; iy < BLOCK_SIZE; iy++) {

                I32_WMV x0 = ppxliErrorQMB [0] + ppxlcRef [0];
                I32_WMV x1 = ppxliErrorQMB [1] + ppxlcRef [1];
                I32_WMV x2 = ppxliErrorQMB [2] + ppxlcRef [2];
                I32_WMV x3 = ppxliErrorQMB [3] + ppxlcRef [3];

#ifdef _FASTMC_               
                I32_WMV d0 = x2 << 16;
                I32_WMV d1 = x3 << 16;
                d0 = d0 | x0;
                d1 = d1 | x1;
                // Check for saturation
                if (((d0 | d1) & 0xff00ff00) == 0)
                {
                    *(U32_WMV *) ppxlcCurrQMB = d0 | (d1 << 8);
                }
                else
                {
                    *(U32_WMV *) ppxlcCurrQMB = packFourPixels (rgiClapTab [x0], rgiClapTab [x1], rgiClapTab [x2], rgiClapTab [x3]);
                }                
#else
                if ((((U32_WMV)x0|(U32_WMV)x1|(U32_WMV)x2|(U32_WMV)x3)&maskStaturation) == 0) {
                    *(U32_WMV *) ppxlcCurrQMB = packFourPixels (x0, x1, x2, x3);
                } else {
                    *(U32_WMV *) ppxlcCurrQMB = packFourPixels (rgiClapTab [x0], rgiClapTab [x1], rgiClapTab [x2], rgiClapTab [x3]);
                }
#endif

                x0 = ppxliErrorQMB [4] + ppxlcRef [4];
                x1 = ppxliErrorQMB [5] + ppxlcRef [5];
                x2 = ppxliErrorQMB [6] + ppxlcRef [6];
                x3 = ppxliErrorQMB [7] + ppxlcRef [7];


#ifdef _FASTMC_
                d0 = x2 << 16;
                d1 = x3 << 16;
                d0 = d0 | x0;
                d1 = d1 | x1;
                // Check for saturation
                if (((d0 | d1) & 0xff00ff00) == 0)
                {
                    *(U32_WMV *) (ppxlcCurrQMB+4) = d0 | (d1 << 8);
                }
                else
                {
                    *(U32_WMV *) (ppxlcCurrQMB+4) = packFourPixels (rgiClapTab [x0], rgiClapTab [x1], rgiClapTab [x2], rgiClapTab [x3]);
                }
#else
                if ((((U32_WMV)x0|(U32_WMV)x1|(U32_WMV)x2|(U32_WMV)x3)&maskStaturation) == 0) {
                    *(U32_WMV *) (ppxlcCurrQMB+4) = packFourPixels (x0, x1, x2, x3);
                } else {
                    *(U32_WMV *) (ppxlcCurrQMB+4) = packFourPixels (rgiClapTab [x0], rgiClapTab [x1], rgiClapTab [x2], rgiClapTab [x3]);       
                }
#endif

                ppxlcRef += iWidthFrm;
                ppxlcCurrQMB += iWidthFrm;
                ppxliErrorQMB += BLOCK_SIZE;
            }
          
        }
        else {  //bXSubPxl && !bYSubPxl
            for (iy = 0; iy < BLOCK_SIZE; iy++){

                I32_WMV x0 = ppxliErrorQMB [0] + ((ppxlcRef [1] + ppxlcRef [0]) >> 1);
                I32_WMV x1 = ppxliErrorQMB [1] + ((ppxlcRef [2] + ppxlcRef [1]) >> 1);
                I32_WMV x2 = ppxliErrorQMB [2] + ((ppxlcRef [3] + ppxlcRef [2]) >> 1);
                I32_WMV x3 = ppxliErrorQMB [3] + ((ppxlcRef [4] + ppxlcRef [3]) >> 1);

#ifdef _FASTMC_
                I32_WMV d0 = x2 << 16;
                I32_WMV d1 = x3 << 16;
                d0 = d0 | x0;
                d1 = d1 | x1;
                // Check for saturation
                if (((d0 | d1) & 0xff00ff00) == 0)
                {
                    *(U32_WMV *) ppxlcCurrQMB = d0 | (d1 << 8);
                }
                else
                {
                    *(U32_WMV *) ppxlcCurrQMB = packFourPixels (rgiClapTab [x0], rgiClapTab [x1], rgiClapTab [x2], rgiClapTab [x3]);            
                }

#else
                if ((((U32_WMV)x0|(U32_WMV)x1|(U32_WMV)x2|(U32_WMV)x3)&maskStaturation) == 0) {
                    *(U32_WMV *) ppxlcCurrQMB = packFourPixels (x0, x1, x2, x3);
                } else {
                    *(U32_WMV *) ppxlcCurrQMB = packFourPixels (rgiClapTab [x0], rgiClapTab [x1], rgiClapTab [x2], rgiClapTab [x3]);            
                }            
#endif

                x0 = ppxliErrorQMB [4] + ((ppxlcRef [5] + ppxlcRef [4]) >> 1);
                x1 = ppxliErrorQMB [5] + ((ppxlcRef [6] + ppxlcRef [5]) >> 1);
                x2 = ppxliErrorQMB [6] + ((ppxlcRef [7] + ppxlcRef [6]) >> 1);
                x3 = ppxliErrorQMB [7] + ((ppxlcRef [8] + ppxlcRef [7]) >> 1);

#ifdef _FASTMC_
                d0 = x2 << 16;
                d1 = x3 << 16;
                d0 = d0 | x0;
                d1 = d1 | x1;
                // Check for saturation
                if (((d0 | d1) & 0xff00ff00) == 0)
                {
                    *(U32_WMV *) (ppxlcCurrQMB+4) = d0 | (d1 << 8);
                }
                else
                {
                    *(U32_WMV *) (ppxlcCurrQMB+4) = packFourPixels (rgiClapTab [x0], rgiClapTab [x1], rgiClapTab [x2], rgiClapTab [x3]);
                }
#else
                if ((((U32_WMV)x0|(U32_WMV)x1|(U32_WMV)x2|(U32_WMV)x3)&maskStaturation) == 0) {
                    *(U32_WMV *) (ppxlcCurrQMB+4) = packFourPixels (x0, x1, x2, x3);
                } else {
                    *(U32_WMV *) (ppxlcCurrQMB+4) = packFourPixels (rgiClapTab [x0], rgiClapTab [x1], rgiClapTab [x2], rgiClapTab [x3]);         
                }
#endif                
                ppxlcRef += iWidthFrm;
                ppxlcCurrQMB += iWidthFrm;
                ppxliErrorQMB += BLOCK_SIZE;
            }
        }
    }
    else {
        const U8_WMV* ppxlcRefBot;
        if (bXEven) {  //!bXSubPxl&& bYSubPxl
            for (iy = 0; iy < BLOCK_SIZE; iy++) {

                ppxlcRefBot = ppxlcRef + iWidthFrm;		//UPln->pixels (xInt,yInt+1);

                I32_WMV x0 = ppxliErrorQMB [0] + ((ppxlcRef [0] + ppxlcRefBot [0]) >> 1);
                I32_WMV x1 = ppxliErrorQMB [1] + ((ppxlcRef [1] + ppxlcRefBot [1]) >> 1);
                I32_WMV x2 = ppxliErrorQMB [2] + ((ppxlcRef [2] + ppxlcRefBot [2]) >> 1);
                I32_WMV x3 = ppxliErrorQMB [3] + ((ppxlcRef [3] + ppxlcRefBot [3]) >> 1);

                
#ifdef _FASTMC_
                {
                    I32_WMV d0 = x2 << 16;
                    I32_WMV d1 = x3 << 16;
                    d0 = d0 | x0;
                    d1 = d1 | x1;
                    // Check for saturation
                    if (((d0 | d1) & 0xff00ff00) == 0)
                    {
                        *(U32_WMV *) ppxlcCurrQMB = d0 | (d1 << 8);
                    }
                    else
                    {
                        *(U32_WMV *) ppxlcCurrQMB = packFourPixels (rgiClapTab [x0], rgiClapTab [x1], rgiClapTab [x2], rgiClapTab [x3]);         
                    }
                }
#else
                if ((((U32_WMV)x0|(U32_WMV)x1|(U32_WMV)x2|(U32_WMV)x3)&maskStaturation) == 0) {
                    *(U32_WMV *) ppxlcCurrQMB = packFourPixels (x0, x1, x2, x3);
                } else {
                    *(U32_WMV *) ppxlcCurrQMB = packFourPixels (rgiClapTab [x0], rgiClapTab [x1], rgiClapTab [x2], rgiClapTab [x3]);         
                }
#endif

                x0 = ppxliErrorQMB [4] + ((ppxlcRef [4] + ppxlcRefBot [4]) >> 1);
                x1 = ppxliErrorQMB [5] + ((ppxlcRef [5] + ppxlcRefBot [5]) >> 1);
                x2 = ppxliErrorQMB [6] + ((ppxlcRef [6] + ppxlcRefBot [6]) >> 1);
                x3 = ppxliErrorQMB [7] + ((ppxlcRef [7] + ppxlcRefBot [7]) >> 1);

#ifdef _FASTMC_
                d0 = x2 << 16;
                d1 = x3 << 16;
                d0 = d0 | x0;
                d1 = d1 | x1;
                // Check for saturation
                if (((d0 | d1) & 0xff00ff00) == 0)
                {
                    *(U32_WMV *) (ppxlcCurrQMB+4) = d0 | (d1 << 8);
                }
                else
                {
                    *(U32_WMV *) (ppxlcCurrQMB+4) = packFourPixels (rgiClapTab [x0], rgiClapTab [x1], rgiClapTab [x2], rgiClapTab [x3]);
                }
#else
                if ((((U32_WMV)x0|(U32_WMV)x1|(U32_WMV)x2|(U32_WMV)x3)&maskStaturation) == 0) {
                    *(U32_WMV *) (ppxlcCurrQMB+4) = packFourPixels (x0, x1, x2, x3);
                } else {
                    *(U32_WMV *) (ppxlcCurrQMB+4) = packFourPixels (rgiClapTab [x0], rgiClapTab [x1], rgiClapTab [x2], rgiClapTab [x3]);            
                }
#endif                
                ppxlcRef = ppxlcRefBot;
                ppxlcCurrQMB += iWidthFrm;
                ppxliErrorQMB += BLOCK_SIZE;
            }
        }
        else { // bXSubPxl && bYSubPxl
            register U32_WMV x;
            for (iy = 0; iy < BLOCK_SIZE; iy++) {
                ppxlcRefBot = ppxlcRef + iWidthFrm;		//UPln->pixels (xInt,yInt+1);

#ifdef _FASTMC_
                {
                    I32_WMV x0, x1, x2, x3, d0, d1;
                    x = ppxlcRef [1] + ppxlcRefBot [1] + 1;
                    
                    x0 = ppxliErrorQMB [0] + ((ppxlcRef [0] + ppxlcRefBot [0] + x) >> 2);	                
                    x1 = ppxliErrorQMB [1] + ((ppxlcRef [2] + ppxlcRefBot [2] + x) >> 2);
                    x = ppxlcRef [3] + ppxlcRefBot [3] + 1;
                    x2 = ppxliErrorQMB [2] + ((ppxlcRef [2] + ppxlcRefBot [2] + x) >> 2);
                    x3 = ppxliErrorQMB [3] + ((ppxlcRef [4] + ppxlcRefBot [4] + x) >> 2);
                    
                    
                    d0 = x2 << 16;
                    d1 = x3 << 16;
                    d0 = d0 | x0;
                    d1 = d1 | x1;
                    // Check for saturation
                    if (((d0 | d1) & 0xff00ff00) == 0)
                    {
                        // If no saturation, write out merged bytes
                        *(U32_WMV *) ppxlcCurrQMB = d0 | (d1 << 8);
                    }
                    else
                    {
                        *(U32_WMV *) ppxlcCurrQMB = packFourPixels (rgiClapTab [x0], rgiClapTab [x1], rgiClapTab [x2], rgiClapTab [x3]);         
                    }
                    
                    x = ppxlcRef [5] + ppxlcRefBot [5] + 1;
                    
                    x0 = ppxliErrorQMB [4] + ((ppxlcRef [4] + ppxlcRefBot [4] + x) >> 2);	                
                    x1 = ppxliErrorQMB [5] + ((ppxlcRef [6] + ppxlcRefBot [6] + x) >> 2);
                    x = ppxlcRef [7] + ppxlcRefBot [7] + 1;
                    x2 = ppxliErrorQMB [6] + ((ppxlcRef [6] + ppxlcRefBot [6] + x) >> 2);
                    x3 = ppxliErrorQMB [7] + ((ppxlcRef [8] + ppxlcRefBot [8] + x) >> 2);
                    
                    d0 = x2 << 16;
                    d1 = x3 << 16;
                    d0 = d0 | x0;
                    d1 = d1 | x1;
                    // Check for saturation
                    if (((d0 | d1) & 0xff00ff00) == 0)
                    {
                        // If no saturation, write out merged bytes
                        *(U32_WMV *) (ppxlcCurrQMB+4) = d0 | (d1 << 8);
                    }
                    else
                    {/*
                     ppxlcCurrQMB [4] = rgiClapTab [x0];
                     ppxlcCurrQMB [5] = rgiClapTab [x1];
                     ppxlcCurrQMB [6] = rgiClapTab [x2];
                     ppxlcCurrQMB [7] = rgiClapTab [x3];
                        */
                        *(U32_WMV *) (ppxlcCurrQMB+4) = packFourPixels (rgiClapTab [x0], rgiClapTab [x1], rgiClapTab [x2], rgiClapTab [x3]);
                    }
                }
#else


                x = ppxlcRef [1] + ppxlcRefBot [1] + 1;
                ppxlcCurrQMB [0] = rgiClapTab [ppxliErrorQMB [0] + ((ppxlcRef [0] + ppxlcRefBot [0] + x) >> 2)];	                
                ppxlcCurrQMB [1] = rgiClapTab [ppxliErrorQMB [1] + ((ppxlcRef [2] + ppxlcRefBot [2] + x) >> 2)];
                x = ppxlcRef [3] + ppxlcRefBot [3] + 1;
                ppxlcCurrQMB [2] = rgiClapTab [ppxliErrorQMB [2] + ((ppxlcRef [2] + ppxlcRefBot [2] + x) >> 2)];
                ppxlcCurrQMB [3] = rgiClapTab [ppxliErrorQMB [3] + ((ppxlcRef [4] + ppxlcRefBot [4] + x) >> 2)];
                x = ppxlcRef [5] + ppxlcRefBot [5] + 1;
                ppxlcCurrQMB [4] = rgiClapTab [ppxliErrorQMB [4] + ((ppxlcRef [4] + ppxlcRefBot [4] + x) >> 2)];	                
                ppxlcCurrQMB [5] = rgiClapTab [ppxliErrorQMB [5] + ((ppxlcRef [6] + ppxlcRefBot [6] + x) >> 2)];
                x = ppxlcRef [7] + ppxlcRefBot [7] + 1;
                ppxlcCurrQMB [6] = rgiClapTab [ppxliErrorQMB [6] + ((ppxlcRef [6] + ppxlcRefBot [6] + x) >> 2)];
                ppxlcCurrQMB [7] = rgiClapTab [ppxliErrorQMB [7] + ((ppxlcRef [8] + ppxlcRefBot [8] + x) >> 2)];


#endif

                ppxlcRef = ppxlcRefBot;
                ppxlcCurrQMB += iWidthFrm;
                ppxliErrorQMB += BLOCK_SIZE;
            }
        }
    }
    FUNCTION_PROFILE_STOP(&fpDecode);
}
#endif

#pragma warning(disable:4244) 
Void_WMV g_MotionCompRndCtrl (
    tWMVDecInternalMember *pWMVDec,
	U8_WMV*             ppxlcPredMB,
	const U8_WMV*       ppxlcRefMB,
    I32_WMV             iWidthFrm,
    Bool_WMV            bInterpolateX,
    Bool_WMV            bInterpolateY,
    I32_WMV             iMixedPelMV	
)
{
    FUNCTION_PROFILE_DECL_START(fpDecode,MOTIONCOMPRNDCTRL_PROFILE);
#ifndef __UNIXVIDEO__

    I32_WMV iy;
    UWide	x01		= (UWide)0x0101010101010101;
    UWide	x7f 	= (UWide)0x7f7f7f7f7f7f7f7f;
    UWide   x3F     = (UWide)0x3F3F3F3F3F3F3F3F;
    UWide   x03     = (UWide)0x0303030303030303;

    UWide   *s1, *s2, *s3, *s4, *d;
    U8_WMV* ppxlcPred;
    const U8_WMV* ppxlcRef;

    if (bInterpolateY) {
        if (bInterpolateX) {  //!bXSubPxl && !bYSubPxl
            for (iy = 0; iy < 8; iy++) {
                //memcpy (ppxlcPredMB, ppxlcRefMB, 8);
                I32_WMV k;
                for (k = 0; k < 8; k += 8) {
                    *(U64 *)(ppxlcPredMB + k) = *(U64 *)(ppxlcRefMB + k);
                }
                ppxlcRefMB += iWidthFrm;
                ppxlcPredMB += iWidthFrm;
            }
        }
        else {  //bXSubPxl && !bYSubPxl
            I32_WMV ix;
            for (ix = 0; ix < 8; ix += sizeof(UWide)) {
                ppxlcPred = ppxlcPredMB+ix;
                ppxlcRef  = ppxlcRefMB+ix;
                for (iy = 0; iy < 8; iy++) {

                    s1 = (UWide*) ppxlcRef;
                    s2 = (UWide*) (ppxlcRef+1);
                    d  = (UWide*) ppxlcPred;

                    *d = (((*s1) >> 1) & x7f) + (((*s2) >> 1) & x7f) + (((*s1) & (*s2)) & x01);

                    ppxlcRef += iWidthFrm;
		            ppxlcPred += iWidthFrm;
	            } 
            }
        }
    }
    else {
        const U8_WMV* ppxlcRefBot;
        if (bInterpolateX) {  //!bXSubPxl&& bYSubPxl
            I32_WMV ix;
            for (ix = 0; ix < 8; ix += sizeof(UWide)) {
                ppxlcPred = ppxlcPredMB+ix;
                ppxlcRef  = ppxlcRefMB+ix;
                for (iy = 0; iy < 8; iy++) {
                    ppxlcRefBot = ppxlcRef + (iWidthFrm*1);		//UPln->pixels (xInt,yInt+1);

                    s1 = (UWide*) ppxlcRef;
                    s2 = (UWide*) ppxlcRefBot;
                    d  = (UWide*) ppxlcPred;

                    *d = (((*s1)>>1)&x7f)+(((*s2)>>1)&x7f)+(((*s1)&(*s2))&x01);

                    ppxlcRef += iWidthFrm;
                    ppxlcPred += iWidthFrm;
	            }
            }
        }
        else { // bXSubPxl && bYSubPxl
            I32_WMV ix;
            for (ix = 0; ix < 8; ix += sizeof(UWide) ) {
                ppxlcPred = ppxlcPredMB;
                ppxlcRef  = ppxlcRefMB;
                for (iy = 0; iy < 8; iy++) {
                    ppxlcRefBot = ppxlcRef + (iWidthFrm*1);

                    s1 = (UWide*) (ppxlcRef+1+ix);
                    s2 = (UWide*) (ppxlcRef+ix);
                    s3 = (UWide*) (ppxlcRefBot+1+ix);
                    s4 = (UWide*) (ppxlcRefBot+ix);
                    d  = (UWide*) (ppxlcPred+ix);

                    *d = (((*s1 >> 2) & x3F) + ((*s2 >> 2) & x3F) +
                          ((*s3 >> 2) & x3F) + ((*s4 >> 2) & x3F))+
                        ((((*s1       & x03) +  (*s2       & x03) +
                           (*s3       & x03) +  (*s4       & x03) + x01) >> 2) & x03);

                    ppxlcRef += iWidthFrm;
                    ppxlcPred += iWidthFrm;
                }
            }
        }
    }
#else // __UNIXVIDEO__
#ifndef _CASIO_VIDEO_
    assert ((8 % 8) == 0);
#endif

    I32_WMV iy;
    U32_WMV x01 = (U32_WMV)0x01010101;
    U32_WMV x7f = (U32_WMV)0x7f7f7f7f;
    U32_WMV x3F = (U32_WMV)0x3F3F3F3F;
    U32_WMV x03 = (U32_WMV)0x03030303;
    U8_WMV* ppxlcPred;
    const U8_WMV* ppxlcRef;
    if ((((U32_WMV) ppxlcRefMB) % 4) == 0) {// aligned
        if (bInterpolateY) {
            if (bInterpolateX) {  //!bXSubPxl && !bYSubPxl
                for (iy = BLOCK_SIZE; iy != 0; iy--) {
                    *(U64 *) ppxlcPredMB = *(U64 *) ppxlcRefMB;
                    ppxlcRefMB += iWidthFrm;
                    ppxlcPredMB += iWidthFrm;
                }
            }
            else {  //bXSubPxl && !bYSubPxl
                I32_WMV ix;
                for (ix = 0; ix < BLOCK_SIZE; ix += 4) {
                    ppxlcPred = ppxlcPredMB + ix;
                    ppxlcRef  = ppxlcRefMB + ix;
                    for (iy = BLOCK_SIZE; iy != 0; iy--) {
                        U32_WMV s1 = *(U32_WMV *) ppxlcRef;
#ifdef BIG_ENDING
                        U32_WMV s2 = (s1 << 8) | ppxlcRef [4];
#else
                        U32_WMV s2 = (s1 >> 8) | (((U32_WMV)ppxlcRef [4])<<24);
#endif
                        *(U32_WMV *) ppxlcPred = ((s1 >> 1) & x7f) + ((s2 >> 1) & x7f) + ((s1 & s2) & x01);
                        ppxlcRef += iWidthFrm;
                        ppxlcPred += iWidthFrm;
                    } 
                }
            }
        }
        else {
            const U8_WMV * ppxlcRefBot;
            if (bInterpolateX) {  //!bXSubPxl&& bYSubPxl
                I32_WMV ix;
                for (ix = 0; ix < BLOCK_SIZE; ix += 4) {
                    ppxlcPred = ppxlcPredMB + ix;
                    ppxlcRef  = ppxlcRefMB + ix;
                    for (iy = BLOCK_SIZE; iy != 0; iy--) {
                        ppxlcRefBot = ppxlcRef + iWidthFrm;		//UPln->pixels (xInt,yInt+1);
                        U32_WMV s1 = *(U32_WMV *) ppxlcRef;
                        U32_WMV s2 = *(U32_WMV *) ppxlcRefBot;
                        *(U32_WMV *) ppxlcPred = ((s1>>1)&x7f)+((s2>>1)&x7f)+((s1&s2)&x01);
                        ppxlcRef += iWidthFrm;
                        ppxlcPred += iWidthFrm;
                    }
                }
            }
            else { // bXSubPxl && bYSubPxl
                I32_WMV ix;
                for (ix = 0; ix < BLOCK_SIZE; ix += 4) {
                    ppxlcPred = ppxlcPredMB + ix;
                    ppxlcRef  = ppxlcRefMB + ix;
                    for (iy = BLOCK_SIZE; iy != 0; iy--) {
                        ppxlcRefBot = ppxlcRef + iWidthFrm;
                        U32_WMV s1 = *(U32_WMV *) ppxlcRef;
#ifdef BIG_ENDING
                        U32_WMV s2 = (s1 << 8) | ppxlcRef [4];
#else
                        U32_WMV s2 = (s1 >> 8) | (((U32_WMV)ppxlcRef [4])<<24);
#endif
                        U32_WMV s3 = *(U32_WMV *) ppxlcRefBot;
#ifdef BIG_ENDING
                        U32_WMV s4 = (s3 << 8) | ppxlcRefBot [4];
#else
                        U32_WMV s4 = (s3 >> 8) | (((U32_WMV)ppxlcRefBot [4])<<24);
#endif
                        *((U32_WMV *)ppxlcPred) = (((s1>>2)&x3F) + ((s2>>2)&x3F) +
                                                  ((s3>>2)&x3F) + ((s4>>2)&x3F))+
                                                ((((s1&x03) + (s2&x03) +
                                                   (s3&x03) + (s4&x03) + x01) >> 2) & x03);
                        ppxlcRef += iWidthFrm;
                        ppxlcPred += iWidthFrm;
                    }
                }
            }
        }
    } else {// not aligned
        if (bInterpolateY) {
            if (bInterpolateX) {  //!bXSubPxl && !bYSubPxl
                for (iy = BLOCK_SIZE; iy != 0; iy--) {
                    memcpy (ppxlcPredMB, ppxlcRefMB, BLOCK_SIZE);
                    ppxlcRefMB += iWidthFrm;
                    ppxlcPredMB += iWidthFrm;
                }
            }
            else {  //bXSubPxl && !bYSubPxl
                for (iy = 0; iy < BLOCK_SIZE; iy++){
                    ppxlcPredMB [0] = (ppxlcRefMB [1] + ppxlcRefMB [0]) >> 1;
                    ppxlcPredMB [1] = (ppxlcRefMB [2] + ppxlcRefMB [1]) >> 1;
                    ppxlcPredMB [2] = (ppxlcRefMB [3] + ppxlcRefMB [2]) >> 1;
                    ppxlcPredMB [3] = (ppxlcRefMB [4] + ppxlcRefMB [3]) >> 1;
                    ppxlcPredMB [4] = (ppxlcRefMB [5] + ppxlcRefMB [4]) >> 1;
                    ppxlcPredMB [5] = (ppxlcRefMB [6] + ppxlcRefMB [5]) >> 1;
                    ppxlcPredMB [6] = (ppxlcRefMB [7] + ppxlcRefMB [6]) >> 1;
                    ppxlcPredMB [7] = (ppxlcRefMB [8] + ppxlcRefMB [7]) >> 1;

                    ppxlcRefMB += iWidthFrm;
                    ppxlcPredMB += iWidthFrm;
                }
            }
        }
        else {
            const U8_WMV * ppxlcRefBot;
            if (bInterpolateX) {  //!bXSubPxl&& bYSubPxl
                for (iy = 0; iy < BLOCK_SIZE; iy++) {
                    ppxlcRefBot = ppxlcRefMB + iWidthFrm;		//UPln->pixels (xInt,yInt+1);

                    ppxlcPredMB [0] = (ppxlcRefMB [0] + ppxlcRefBot [0]) >> 1;
                    ppxlcPredMB [1] = (ppxlcRefMB [1] + ppxlcRefBot [1]) >> 1;
                    ppxlcPredMB [2] = (ppxlcRefMB [2] + ppxlcRefBot [2]) >> 1;
                    ppxlcPredMB [3] = (ppxlcRefMB [3] + ppxlcRefBot [3]) >> 1;
                    ppxlcPredMB [4] = (ppxlcRefMB [4] + ppxlcRefBot [4]) >> 1;
                    ppxlcPredMB [5] = (ppxlcRefMB [5] + ppxlcRefBot [5]) >> 1;
                    ppxlcPredMB [6] = (ppxlcRefMB [6] + ppxlcRefBot [6]) >> 1;
                    ppxlcPredMB [7] = (ppxlcRefMB [7] + ppxlcRefBot [7]) >> 1;

                    ppxlcRefMB = ppxlcRefBot;
                    ppxlcPredMB += iWidthFrm;
                }
            }
            else { // bXSubPxl && bYSubPxl
                register U32_WMV x, y;
                for (iy = 0; iy < BLOCK_SIZE; iy++) {
                    ppxlcRefBot = ppxlcRefMB + iWidthFrm;		//UPln->pixels (xInt,yInt+1);

                    x = ppxlcRefMB [1] + ppxlcRefBot [1] + 1;
                    ppxlcPredMB [0] = (x + ppxlcRefMB [0] + ppxlcRefBot [0]) >> 2;
                    y = ppxlcRefMB [2] + ppxlcRefBot [2];
                    ppxlcPredMB [1] = (x + y) >> 2;
                    x = y + 1;
                    y = ppxlcRefMB [3] + ppxlcRefBot [3];
                    ppxlcPredMB [2] = (x + y) >> 2;
                    x = y + 1;
                    y = ppxlcRefMB [4] + ppxlcRefBot [4];
                    ppxlcPredMB [3] = (x + y) >> 2;
                    x = y + 1;
                    y = ppxlcRefMB [5] + ppxlcRefBot [5];
                    ppxlcPredMB [4] = (x + y) >> 2;
                    x = y + 1;
                    y = ppxlcRefMB [6] + ppxlcRefBot [6];
                    ppxlcPredMB [5] = (x + y) >> 2;
                    x = y + 1;
                    y = ppxlcRefMB [7] + ppxlcRefBot [7];
                    ppxlcPredMB [6] = (x + y) >> 2;
                    x = y + 1;
                    ppxlcPredMB [7] = (x + ppxlcRefMB [8] + ppxlcRefBot [8]) >> 2;

                    ppxlcRefMB = ppxlcRefBot;
                    ppxlcPredMB += iWidthFrm;
                }
            }
        }
    }
#endif
    FUNCTION_PROFILE_STOP(&fpDecode);
}
#pragma warning(default:4244) 


#ifdef _USE_PORTABLE_C_

Void_WMV g_MotionCompAndAddError (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* ppxlcCurrQMB,
    const Buffer __huge* ppxliErrorBuf, const U8_WMV* ppxlcRef,
    I32_WMV iWidthFrm, Bool_WMV bXEven, Bool_WMV bYEven,
    I32_WMV iMixedPelMV	
)
{
    const U8_WMV* rgiClapTab = pWMVDec->m_rgiClapTabDec; 

//#ifndef _CASIO_VIDEO_
//    assert (rgiClapTab != NULL_WMV);
//#endif
    FUNCTION_PROFILE_DECL_START(fpDecode,MOTIONCOMPADDERROR_PROFILE);

    const PixelI32 *ppxliErrorQMB = ppxliErrorBuf->i32;

    
    I32_WMV iy;
    if (bYEven) {
        if (bXEven) {  //!bXSubPxl && !bYSubPxl
            for (iy = 0; iy < BLOCK_SIZE; iy++) {


                I32_WMV x0 = ppxliErrorQMB [0] + ppxlcRef [0];
                I32_WMV x1 = ppxliErrorQMB [1] + ppxlcRef [1];
                I32_WMV x2 = ppxliErrorQMB [2] + ppxlcRef [2];
                I32_WMV x3 = ppxliErrorQMB [3] + ppxlcRef [3];

#ifdef _FASTMC_
                I32_WMV d0 = x2 << 16;
                I32_WMV d1 = x3 << 16;
                d0 = d0 | x0;
                d1 = d1 | x1;
                // Check for saturation
                if (((d0 | d1) & 0xff00ff00) == 0)
                {
                    // If no saturation, write out merged bytes
                    *(U32_WMV *) ppxlcCurrQMB = d0 | (d1 << 8);
                }
                else
                {
                    *(U32_WMV *) ppxlcCurrQMB = packFourPixels (rgiClapTab [x0], rgiClapTab [x1], rgiClapTab [x2], rgiClapTab [x3]);
                }
#else
                if ((((U32_WMV)x0|(U32_WMV)x1|(U32_WMV)x2|(U32_WMV)x3)&maskStaturation) == 0) {
                    *(U32_WMV *) ppxlcCurrQMB = packFourPixels (x0, x1, x2, x3);
                } else {
                    *(U32_WMV *) ppxlcCurrQMB = packFourPixels (rgiClapTab [x0], rgiClapTab [x1], rgiClapTab [x2], rgiClapTab [x3]);          
                }
#endif

                x0 = ppxliErrorQMB [4] + ppxlcRef [4];
                x1 = ppxliErrorQMB [5] + ppxlcRef [5];
                x2 = ppxliErrorQMB [6] + ppxlcRef [6];
                x3 = ppxliErrorQMB [7] + ppxlcRef [7];


#ifdef _FASTMC_
                d0 = x2 << 16;
                d1 = x3 << 16;
                d0 = d0 | x0;
                d1 = d1 | x1;
                // Check for saturation
                if (((d0 | d1) & 0xff00ff00) == 0)
                {
                    // If no saturation, write out merged bytes
                    *(U32_WMV *) (ppxlcCurrQMB+4) = d0 | (d1 << 8);
                }
                else
                {
                    *(U32_WMV *) (ppxlcCurrQMB+4) = packFourPixels (rgiClapTab [x0], rgiClapTab [x1], rgiClapTab [x2], rgiClapTab [x3]);      
                }
#else
                if ((((U32_WMV)x0|(U32_WMV)x1|(U32_WMV)x2|(U32_WMV)x3)&maskStaturation) == 0) {
                    *(U32_WMV *) (ppxlcCurrQMB+4) = packFourPixels (x0, x1, x2, x3);
                } else {
                    *(U32_WMV *) (ppxlcCurrQMB+4) = packFourPixels (rgiClapTab [x0], rgiClapTab [x1], rgiClapTab [x2], rgiClapTab [x3]);      
                }
#endif
                ppxlcRef += iWidthFrm;
                ppxlcCurrQMB += iWidthFrm;
                ppxliErrorQMB += BLOCK_SIZE;
            }
        }
        else {  //bXSubPxl && !bYSubPxl
            for (iy = 0; iy < BLOCK_SIZE; iy++){
#ifdef _FASTMC_
                register U32_WMV x;
                x = ppxlcRef [1] + 1;

                I32_WMV x0 = ppxliErrorQMB [0] + ((x + ppxlcRef [0]) >> 1);
                I32_WMV x1 = ppxliErrorQMB [1] + ((x + ppxlcRef [2]) >> 1);
                x = ppxlcRef [3] + 1;
                I32_WMV x2 = ppxliErrorQMB [2] + ((x + ppxlcRef [2]) >> 1);
                I32_WMV x3 = ppxliErrorQMB [3] + ((x + ppxlcRef [4]) >> 1);

                I32_WMV d0 = x2 << 16;
                I32_WMV d1 = x3 << 16;
                d0 = d0 | x0;
                d1 = d1 | x1;
                // Check for saturation
                if (((d0 | d1) & 0xff00ff00) == 0)
                {
                    // If no saturation, write out merged bytes
                    *(U32_WMV *) ppxlcCurrQMB = d0 | (d1 << 8);
                }
                else
                {
                    *(U32_WMV *) ppxlcCurrQMB = packFourPixels (rgiClapTab [x0], rgiClapTab [x1], rgiClapTab [x2], rgiClapTab [x3]);      
                }

                x = ppxlcRef [5] + 1;


                x0 = ppxliErrorQMB [4] + ((x + ppxlcRef [4]) >> 1);
                x1 = ppxliErrorQMB [5] + ((x + ppxlcRef [6]) >> 1);
                x = ppxlcRef [7] + 1;
                x2 = ppxliErrorQMB [6] + ((x + ppxlcRef [6]) >> 1);
                x3 = ppxliErrorQMB [7] + ((x + ppxlcRef [8]) >> 1);

                d0 = x2 << 16;
                d1 = x3 << 16;
                d0 = d0 | x0;
                d1 = d1 | x1;
                // Check for saturation
                if (((d0 | d1) & 0xff00ff00) == 0)
                {
                    // If no saturation, write out merged bytes
                    *(U32_WMV *) (ppxlcCurrQMB+4) = d0 | (d1 << 8);
                }
                else
                {
                    *(U32_WMV *) (ppxlcCurrQMB+4) = packFourPixels (rgiClapTab [x0], rgiClapTab [x1], rgiClapTab [x2], rgiClapTab [x3]);      
                }
#else

                I32_WMV x0 = ppxliErrorQMB [0] + ((ppxlcRef [1] + ppxlcRef [0] + 1) >> 1);
                I32_WMV x1 = ppxliErrorQMB [1] + ((ppxlcRef [2] + ppxlcRef [1] + 1) >> 1);
                I32_WMV x2 = ppxliErrorQMB [2] + ((ppxlcRef [3] + ppxlcRef [2] + 1) >> 1);
                I32_WMV x3 = ppxliErrorQMB [3] + ((ppxlcRef [4] + ppxlcRef [3] + 1) >> 1);

                if ((((U32_WMV)x0|(U32_WMV)x1|(U32_WMV)x2|(U32_WMV)x3)&maskStaturation) == 0) {
                    *(U32_WMV *) ppxlcCurrQMB = packFourPixels (x0, x1, x2, x3);
                } else {
                    *(U32_WMV *) ppxlcCurrQMB = packFourPixels (rgiClapTab [x0], rgiClapTab [x1], rgiClapTab [x2], rgiClapTab [x3]);      
                }          

                x0 = ppxliErrorQMB [4] + ((ppxlcRef [5] + ppxlcRef [4] + 1) >> 1);
                x1 = ppxliErrorQMB [5] + ((ppxlcRef [6] + ppxlcRef [5] + 1) >> 1);
                x2 = ppxliErrorQMB [6] + ((ppxlcRef [7] + ppxlcRef [6] + 1) >> 1);
                x3 = ppxliErrorQMB [7] + ((ppxlcRef [8] + ppxlcRef [7] + 1) >> 1);

                if ((((U32_WMV)x0|(U32_WMV)x1|(U32_WMV)x2|(U32_WMV)x3)&maskStaturation) == 0) {
                    *(U32_WMV *) (ppxlcCurrQMB+4) = packFourPixels (x0, x1, x2, x3); 
                } else {
                    *(U32_WMV *) (ppxlcCurrQMB+4) = packFourPixels (rgiClapTab [x0], rgiClapTab [x1], rgiClapTab [x2], rgiClapTab [x3]);          
                }
#endif
                
                ppxlcRef += iWidthFrm;
                ppxlcCurrQMB += iWidthFrm;
                ppxliErrorQMB += BLOCK_SIZE;
            }
        }
    }
    else {
        const U8_WMV* ppxlcRefBot;
        if (bXEven) {  //!bXSubPxl&& bYSubPxl
            for (iy = 0; iy < BLOCK_SIZE; iy++) {

                ppxlcRefBot = ppxlcRef + iWidthFrm;		//UPln->pixels (xInt,yInt+1);

                I32_WMV x0 = ppxliErrorQMB [0] + ((ppxlcRef [0] + ppxlcRefBot [0] + 1) >> 1);
                I32_WMV x1 = ppxliErrorQMB [1] + ((ppxlcRef [1] + ppxlcRefBot [1] + 1) >> 1);
                I32_WMV x2 = ppxliErrorQMB [2] + ((ppxlcRef [2] + ppxlcRefBot [2] + 1) >> 1);
                I32_WMV x3 = ppxliErrorQMB [3] + ((ppxlcRef [3] + ppxlcRefBot [3] + 1) >> 1);

                if ((((U32_WMV)x0|(U32_WMV)x1|(U32_WMV)x2|(U32_WMV)x3)&maskStaturation) == 0) {
                    *(U32_WMV *) ppxlcCurrQMB = packFourPixels (x0, x1, x2, x3);
                } else {
                    *(U32_WMV *) ppxlcCurrQMB = packFourPixels (rgiClapTab [x0], rgiClapTab [x1], rgiClapTab [x2], rgiClapTab [x3]);         
                }

                x0 = ppxliErrorQMB [4] + ((ppxlcRef [4] + ppxlcRefBot [4] + 1) >> 1);
                x1 = ppxliErrorQMB [5] + ((ppxlcRef [5] + ppxlcRefBot [5] + 1) >> 1);
                x2 = ppxliErrorQMB [6] + ((ppxlcRef [6] + ppxlcRefBot [6] + 1) >> 1);
                x3 = ppxliErrorQMB [7] + ((ppxlcRef [7] + ppxlcRefBot [7] + 1) >> 1);

                if ((((U32_WMV)x0|(U32_WMV)x1|(U32_WMV)x2|(U32_WMV)x3)&maskStaturation) == 0) {
                    *(U32_WMV *) (ppxlcCurrQMB+4) = packFourPixels (x0, x1, x2, x3);
                } else {
                    *(U32_WMV *) (ppxlcCurrQMB+4) = packFourPixels (rgiClapTab [x0], rgiClapTab [x1], rgiClapTab [x2], rgiClapTab [x3]);   
                }
                 
                ppxlcRef = ppxlcRefBot;
                ppxlcCurrQMB += iWidthFrm;
                ppxliErrorQMB += BLOCK_SIZE;
            }
        }
        else { // bXSubPxl && bYSubPxl
            register U32_WMV x, y;
            for (iy = 0; iy < BLOCK_SIZE; iy++) {
                ppxlcRefBot = ppxlcRef + iWidthFrm;		//UPln->pixels (xInt,yInt+1);

#ifdef _FASTMC_


				x = ppxlcRef [1] + ppxlcRefBot [1] + 2;
                I32_WMV x0 = ppxliErrorQMB [0] + ((x + ppxlcRef [0] + ppxlcRefBot [0]) >> 2);
                y = ppxlcRef [2] + ppxlcRefBot [2];
                I32_WMV x1 = ppxliErrorQMB [1] + ((x + y) >> 2);
                x = y + 2;
                y = ppxlcRef [3] + ppxlcRefBot [3];
                I32_WMV x2 = ppxliErrorQMB [2] + ((x + y) >> 2);
                x = y + 2;
                y = ppxlcRef [4] + ppxlcRefBot [4];
                I32_WMV x3 = ppxliErrorQMB [3] + ((x + y) >> 2);


                I32_WMV d0 = x2 << 16;
                I32_WMV d1 = x3 << 16;
                d0 = d0 | x0;
                d1 = d1 | x1;
                // Check for saturation
                if (((d0 | d1) & 0xff00ff00) == 0)
                {
                    // If no saturation, write out merged bytes
                    *(U32_WMV *) ppxlcCurrQMB = d0 | (d1 << 8);
                }
                else
                {
                    *(U32_WMV *) ppxlcCurrQMB = packFourPixels (rgiClapTab [x0], rgiClapTab [x1], rgiClapTab [x2], rgiClapTab [x3]);      
                }

                x = y + 2;

                y = ppxlcRef [5] + ppxlcRefBot [5];
                x0 = ppxliErrorQMB [4] + ((x + y) >> 2);
                x = y + 2;
                y = ppxlcRef [6] + ppxlcRefBot [6];
                x1 = ppxliErrorQMB [5] + ((x + y) >> 2);
                x = y + 2;
                y = ppxlcRef [7] + ppxlcRefBot [7];
                x2 = ppxliErrorQMB [6] + ((x + y) >> 2);
                x = y + 2;
                x3 = ppxliErrorQMB [7] + ((x + ppxlcRef [8] + ppxlcRefBot [8]) >> 2);


                d0 = x2 << 16;
                d1 = x3 << 16;
                d0 = d0 | x0;
                d1 = d1 | x1;
                // Check for saturation
                if (((d0 | d1) & 0xff00ff00) == 0)
                {
                    // If no saturation, write out merged bytes
                    *(U32_WMV *) (ppxlcCurrQMB+4) = d0 | (d1 << 8);
                }
                else
                {
                    ppxlcCurrQMB [4] = rgiClapTab [x0];
                    ppxlcCurrQMB [5] = rgiClapTab [x1];
                    ppxlcCurrQMB [6] = rgiClapTab [x2];
                    ppxlcCurrQMB [7] = rgiClapTab [x3];
                }
#else



                x = ppxlcRef [1] + ppxlcRefBot [1] + 2;
                ppxlcCurrQMB [0] = rgiClapTab [ppxliErrorQMB [0] + ((x + ppxlcRef [0] + ppxlcRefBot [0]) >> 2)];
                y = ppxlcRef [2] + ppxlcRefBot [2];
                ppxlcCurrQMB [1] = rgiClapTab [ppxliErrorQMB [1] + ((x + y) >> 2)];
                x = y + 2;
                y = ppxlcRef [3] + ppxlcRefBot [3];
                ppxlcCurrQMB [2] = rgiClapTab [ppxliErrorQMB [2] + ((x + y) >> 2)];
                x = y + 2;
                y = ppxlcRef [4] + ppxlcRefBot [4];
                ppxlcCurrQMB [3] = rgiClapTab [ppxliErrorQMB [3] + ((x + y) >> 2)];
                x = y + 2;
                y = ppxlcRef [5] + ppxlcRefBot [5];
                ppxlcCurrQMB [4] = rgiClapTab [ppxliErrorQMB [4] + ((x + y) >> 2)];
                x = y + 2;
                y = ppxlcRef [6] + ppxlcRefBot [6];
                ppxlcCurrQMB [5] = rgiClapTab [ppxliErrorQMB [5] + ((x + y) >> 2)];
                x = y + 2;
                y = ppxlcRef [7] + ppxlcRefBot [7];
                ppxlcCurrQMB [6] = rgiClapTab [ppxliErrorQMB [6] + ((x + y) >> 2)];
                x = y + 2;
                ppxlcCurrQMB [7] = rgiClapTab [ppxliErrorQMB [7] + ((x + ppxlcRef [8] + ppxlcRefBot [8]) >> 2)];

#endif

                ppxlcRef = ppxlcRefBot;
                ppxlcCurrQMB += iWidthFrm;
                ppxliErrorQMB += BLOCK_SIZE;
            }
        }
    }
    FUNCTION_PROFILE_STOP(&fpDecode);
}

#endif

#pragma warning(disable:4244) 

Void_WMV g_MotionComp (
    tWMVDecInternalMember *pWMVDec,
	U8_WMV*              ppxlcPredMB,
	const U8_WMV*        ppxlcRefMB,
    I32_WMV                  iWidthFrm,
    Bool_WMV                 bInterpolateX,
    Bool_WMV                 bInterpolateY,
    I32_WMV iMixedPelMV	
)
{
    FUNCTION_PROFILE_DECL_START(fpDecode,MOTIONCOMP_PROFILE);
#ifndef __UNIXVIDEO__

    I32_WMV iy;
    UWide	x01		= (UWide)0x0101010101010101;
    UWide	x7f 	= (UWide)0x7f7f7f7f7f7f7f7f;
    UWide   x3F     = (UWide)0x3F3F3F3F3F3F3F3F;
    UWide   x03     = (UWide)0x0303030303030303;
    UWide   x02     = (UWide)0x0202020202020202;

    UWide   *s1, *s2, *s3, *s4, *d;

    U8_WMV*       ppxlcPred;
    const U8_WMV* ppxlcRef;

    if (bInterpolateY) {
        if (bInterpolateX) {  //!bXSubPxl && !bYSubPxl
            for (iy = 0; iy < 8; iy++) {
                //memcpy (ppxlcPredMB, ppxlcRefMB, 8);
                for (I32_WMV k = 0; k < 8; k += 8) {
                	*(U64 *)(ppxlcPredMB + k) = *(U64 *)(ppxlcRefMB + k);
                }
                ppxlcRefMB += iWidthFrm;
                ppxlcPredMB += iWidthFrm;
            }
        }
        else {  //bXSubPxl && !bYSubPxl
            int ix;
            for (ix = 0; ix < 8; ix += sizeof(UWide)) {
                ppxlcPred = ppxlcPredMB+ix;
                ppxlcRef  = ppxlcRefMB+ix;
                for (iy = 0; iy < 8; iy++) {
                    s1 = (UWide*) ppxlcRef;
                    s2 = (UWide*) (ppxlcRef+1);
                    d  = (UWide*) ppxlcPred;

                    *d = (((*s1) >> 1) & x7f) + (((*s2) >> 1) & x7f) + (((*s1) | (*s2)) & x01);

                    ppxlcRef += iWidthFrm;
		            ppxlcPred += iWidthFrm;
	            }
            }
        }
    }
    else {
        const U8_WMV* ppxlcRefBot;
        I32_WMV ioffset = iWidthFrm*1;
        if (bInterpolateX) {  //!bXSubPxl&& bYSubPxl
            for (int ix = 0; ix < 8; ix += sizeof(UWide)) {
                ppxlcPred = ppxlcPredMB+ix;
                ppxlcRef  = ppxlcRefMB+ix;
                for (iy = 0; iy < 8; iy++) {
                    ppxlcRefBot = ppxlcRef + ioffset; //UPln->pixels (xInt,yInt+1);

                    s1 = (UWide*) ppxlcRef;
                    s2 = (UWide*) ppxlcRefBot;
                    d  = (UWide*) ppxlcPred;
                    *d = (((*s1)>>1)&x7f)+(((*s2)>>1)&x7f)+(((*s1)|(*s2))&x01);

                    ppxlcRef += iWidthFrm;
                    ppxlcPred += iWidthFrm;
                }
            }
        }
        else { // bXSubPxl && bYSubPxl
            for (int ix = 0; ix < 8; ix += sizeof(UWide) ) {
                ppxlcPred = ppxlcPredMB;
                ppxlcRef  = ppxlcRefMB;
                for (iy = 0; iy < 8; iy++) {
                    ppxlcRefBot = ppxlcRef + ioffset;

                    s1 = (UWide*) (ppxlcRef+1+ix);
                    s2 = (UWide*) (ppxlcRef+ix);
                    s3 = (UWide*) (ppxlcRefBot+1+ix);
                    s4 = (UWide*) (ppxlcRefBot+ix);
                    d  = (UWide*) (ppxlcPred+ix);

                    *d = (((*s1 >> 2) & x3F) + ((*s2 >> 2) & x3F) +
                          ((*s3 >> 2) & x3F) + ((*s4 >> 2) & x3F))+
                        ((((*s1       & x03) +  (*s2       & x03) +
                           (*s3       & x03) +  (*s4       & x03) + x02) >> 2) & x03);

                    ppxlcRef += iWidthFrm;
                    ppxlcPred += iWidthFrm;
                }
            }
        }
    }
#endif
#ifdef __UNIXVIDEO__

    I32_WMV iy;
    U32_WMV x01 = (U32_WMV)0x01010101;
    U32_WMV x7f = (U32_WMV)0x7f7f7f7f;
    U32_WMV x3F = (U32_WMV)0x3F3F3F3F;
    U32_WMV x03 = (U32_WMV)0x03030303;
    U32_WMV x02 = (U32_WMV)0x02020202;
    U8_WMV* ppxlcPred;
    const U8_WMV* ppxlcRef;
    if ((((U32_WMV) ppxlcRefMB) % 4) == 0) {// aligned
        if (bInterpolateY) {
            if (bInterpolateX) {  //!bXSubPxl && !bYSubPxl
                for (iy = BLOCK_SIZE; iy != 0; iy--) {
                    *(U64 *) ppxlcPredMB = *(U64 *) ppxlcRefMB;
                    ppxlcRefMB += iWidthFrm;
                    ppxlcPredMB += iWidthFrm;
                }
            }
            else {  //bXSubPxl && !bYSubPxl
                for (I32_WMV ix = 0; ix < BLOCK_SIZE; ix += 4) {
                    ppxlcPred = ppxlcPredMB + ix;
                    ppxlcRef  = ppxlcRefMB + ix;
                    for (iy = BLOCK_SIZE; iy != 0; iy--) {
                        U32_WMV s1 = *(U32_WMV *) ppxlcRef;
#ifdef BIG_ENDING
                        U32_WMV s2 = (s1 << 8) | ppxlcRef [4];
#else
                        U32_WMV s2 = (s1 >> 8) | (((U32_WMV)ppxlcRef [4])<<24);
#endif
                        *(U32_WMV *) ppxlcPred = ((s1 >> 1) & x7f) + ((s2 >> 1) & x7f) + ((s1 | s2) & x01);
                        ppxlcRef += iWidthFrm;
		                ppxlcPred += iWidthFrm;
	                }
                }
            }
        }
        else {
            const U8_WMV * ppxlcRefBot;
            if (bInterpolateX) {  //!bXSubPxl&& bYSubPxl
                for (I32_WMV ix = 0; ix < BLOCK_SIZE; ix += 4) {
                    ppxlcPred = ppxlcPredMB + ix;
                    ppxlcRef  = ppxlcRefMB + ix;
                    for (iy = BLOCK_SIZE; iy != 0; iy--) {
                        ppxlcRefBot = ppxlcRef + iWidthFrm;		//UPln->pixels (xInt,yInt+1);
                        U32_WMV s1 = *(U32_WMV *) ppxlcRef;
                        U32_WMV s2 = *(U32_WMV *) ppxlcRefBot;
                        *(U32_WMV *) ppxlcPred = ((s1>>1)&x7f)+((s2>>1)&x7f)+((s1|s2)&x01);
                        ppxlcRef += iWidthFrm;
                        ppxlcPred += iWidthFrm;
                    }
                }
            }
            else { // bXSubPxl && bYSubPxl
                for (I32_WMV ix = 0; ix < BLOCK_SIZE; ix += 4) {
                    ppxlcPred = ppxlcPredMB + ix;
                    ppxlcRef  = ppxlcRefMB + ix;
                    for (iy = BLOCK_SIZE; iy != 0; iy--) {
                        ppxlcRefBot = ppxlcRef + iWidthFrm;
                        U32_WMV s1 = *(U32_WMV *) ppxlcRef;
#ifdef BIG_ENDING
                        U32_WMV s2 = (s1 << 8) | ppxlcRef [4];
#else
                        U32_WMV s2 = (s1 >> 8) | (((U32_WMV)ppxlcRef [4])<<24);
#endif
                        U32_WMV s3 = *(U32_WMV *) ppxlcRefBot;
#ifdef BIG_ENDING
                        U32_WMV s4 = (s3 << 8) | ppxlcRefBot [4];
#else
                        U32_WMV s4 = (s3 >> 8) | (((U32_WMV)ppxlcRefBot [4])<<24);
#endif
                        *((U32_WMV *)ppxlcPred) = (((s1>>2)&x3F) + ((s2>>2)&x3F) +
                                                  ((s3>>2)&x3F) + ((s4>>2)&x3F))+
                                                ((((s1&x03) + (s2&x03) +
                                                   (s3&x03) + (s4&x03) + x02) >> 2) & x03);
                        ppxlcRef += iWidthFrm;
                        ppxlcPred += iWidthFrm;
                    }
                }
            }
        }
    } else {// not aligned
        if (bInterpolateY) {
            if (bInterpolateX) {  //!bXSubPxl && !bYSubPxl
                for (iy = BLOCK_SIZE; iy != 0; iy--) {
                    memcpy (ppxlcPredMB, ppxlcRefMB, BLOCK_SIZE);
                    ppxlcRefMB += iWidthFrm;
                    ppxlcPredMB += iWidthFrm;
                }
            }
            else {  //bXSubPxl && !bYSubPxl
                for (iy = 0; iy < BLOCK_SIZE; iy++){
#ifdef _FASTMC_
                    register U32_WMV x;
                    x = ppxlcRefMB [1] + 1;
                    ppxlcPredMB [0] = (x + ppxlcRefMB [0]) >> 1;
                    ppxlcPredMB [1] = (x + ppxlcRefMB [2]) >> 1;
                    x = ppxlcRefMB [3] + 1;
                    ppxlcPredMB [2] = (x + ppxlcRefMB [2]) >> 1;
                    ppxlcPredMB [3] = (x + ppxlcRefMB [4]) >> 1;
                    x = ppxlcRefMB [5] + 1;
                    ppxlcPredMB [4] = (x + ppxlcRefMB [4]) >> 1;
                    ppxlcPredMB [5] = (x + ppxlcRefMB [6]) >> 1;
                    x = ppxlcRefMB [7] + 1;
                    ppxlcPredMB [6] = (x + ppxlcRefMB [6]) >> 1;
                    ppxlcPredMB [7] = (x + ppxlcRefMB [8]) >> 1;

#else
                    ppxlcPredMB [0] = (ppxlcRefMB [1] + ppxlcRefMB [0] + 1) >> 1;
                    ppxlcPredMB [1] = (ppxlcRefMB [2] + ppxlcRefMB [1] + 1) >> 1;
                    ppxlcPredMB [2] = (ppxlcRefMB [3] + ppxlcRefMB [2] + 1) >> 1;
                    ppxlcPredMB [3] = (ppxlcRefMB [4] + ppxlcRefMB [3] + 1) >> 1;
                    ppxlcPredMB [4] = (ppxlcRefMB [5] + ppxlcRefMB [4] + 1) >> 1;
                    ppxlcPredMB [5] = (ppxlcRefMB [6] + ppxlcRefMB [5] + 1) >> 1;
                    ppxlcPredMB [6] = (ppxlcRefMB [7] + ppxlcRefMB [6] + 1) >> 1;
                    ppxlcPredMB [7] = (ppxlcRefMB [8] + ppxlcRefMB [7] + 1) >> 1;
#endif

                    ppxlcRefMB += iWidthFrm;
                    ppxlcPredMB += iWidthFrm;
                }
            }
        }
        else {
            const U8_WMV * ppxlcRefBot;
            if (bInterpolateX) {  //!bXSubPxl&& bYSubPxl
                for (iy = 0; iy < BLOCK_SIZE; iy++) {

                    ppxlcRefBot = ppxlcRefMB + iWidthFrm;		//UPln->pixels (xInt,yInt+1);

                    ppxlcPredMB [0] = (ppxlcRefMB [0] + ppxlcRefBot [0] + 1) >> 1;
                    ppxlcPredMB [1] = (ppxlcRefMB [1] + ppxlcRefBot [1] + 1) >> 1;
                    ppxlcPredMB [2] = (ppxlcRefMB [2] + ppxlcRefBot [2] + 1) >> 1;
                    ppxlcPredMB [3] = (ppxlcRefMB [3] + ppxlcRefBot [3] + 1) >> 1;
                    ppxlcPredMB [4] = (ppxlcRefMB [4] + ppxlcRefBot [4] + 1) >> 1;
                    ppxlcPredMB [5] = (ppxlcRefMB [5] + ppxlcRefBot [5] + 1) >> 1;
                    ppxlcPredMB [6] = (ppxlcRefMB [6] + ppxlcRefBot [6] + 1) >> 1;
                    ppxlcPredMB [7] = (ppxlcRefMB [7] + ppxlcRefBot [7] + 1) >> 1;

                    ppxlcRefMB = ppxlcRefBot;
                    ppxlcPredMB += iWidthFrm;
                }
            }
            else { // bXSubPxl && bYSubPxl
                register U32_WMV x, y;
                for (iy = 0; iy < BLOCK_SIZE; iy++) {
                    ppxlcRefBot = ppxlcRefMB + iWidthFrm;		//UPln->pixels (xInt,yInt+1);

                    x = ppxlcRefMB [1] + ppxlcRefBot [1] + 2;
                    ppxlcPredMB [0] = (x + ppxlcRefMB [0] + ppxlcRefBot [0]) >> 2;
                    y = ppxlcRefMB [2] + ppxlcRefBot [2];
                    ppxlcPredMB [1] = (x + y) >> 2;
                    x = y + 2;
                    y = ppxlcRefMB [3] + ppxlcRefBot [3];
                    ppxlcPredMB [2] = (x + y) >> 2;
                    x = y + 2;
                    y = ppxlcRefMB [4] + ppxlcRefBot [4];
                    ppxlcPredMB [3] = (x + y) >> 2;
                    x = y + 2;
                    y = ppxlcRefMB [5] + ppxlcRefBot [5];
                    ppxlcPredMB [4] = (x + y) >> 2;
                    x = y + 2;
                    y = ppxlcRefMB [6] + ppxlcRefBot [6];
                    ppxlcPredMB [5] = (x + y) >> 2;
                    x = y + 2;
                    y = ppxlcRefMB [7] + ppxlcRefBot [7];
                    ppxlcPredMB [6] = (x + y) >> 2;
                    x = y + 2;
                    ppxlcPredMB [7] = (x + ppxlcRefMB [8] + ppxlcRefBot [8]) >> 2;

                    ppxlcRefMB = ppxlcRefBot;
                    ppxlcPredMB += iWidthFrm;
                }
            }
        }
    }
#endif // __UNIXVIDEO__
    FUNCTION_PROFILE_STOP(&fpDecode);
}
#endif //_EMB_WMV2_

#pragma warning(default:4244) 
#ifndef OPT_ZEROMOTION_ARM
Void_WMV g_MotionCompZeroMotion_WMV (
    U8_WMV* ppxliCurrQYMB, 
    U8_WMV* ppxliCurrQUMB, 
    U8_WMV* ppxliCurrQVMB,
    const U8_WMV* ppxliRefYMB, 
    const U8_WMV* ppxliRefUMB, 
    const U8_WMV* ppxliRefVMB,
    I32_WMV iWidthY,
    I32_WMV iWidthUV
)
{
#if !defined(_WIN32)
    assert(MB_SIZE < UINT_MAX);
    assert(BLOCK_SIZE < UINT_MAX);
#endif
    for (I32_WMV i = 0; i < BLOCK_SIZE; i++) {
        *(U64 *) ppxliCurrQYMB = *(U64 *) ppxliRefYMB;
        *(U64 *) (ppxliCurrQYMB + 8) = *(U64 *) (ppxliRefYMB + 8);
        ppxliCurrQYMB += iWidthY;
        ppxliRefYMB += iWidthY;
        *(U64 *) ppxliCurrQYMB = *(U64 *) ppxliRefYMB;
        *(U64 *) (ppxliCurrQYMB + 8) = *(U64 *) (ppxliRefYMB + 8);
        ppxliCurrQYMB += iWidthY;  
        ppxliRefYMB += iWidthY;
        *(U64 *) ppxliCurrQUMB = *(U64 *) ppxliRefUMB;
        ppxliCurrQUMB += iWidthUV;
        ppxliRefUMB += iWidthUV;
        *(U64 *) ppxliCurrQVMB = *(U64 *) ppxliRefVMB;
        ppxliCurrQVMB += iWidthUV;
        ppxliRefVMB += iWidthUV;
    }
}
#endif //OPT_ZEROMOTION_ARM
