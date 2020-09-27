//+-------------------------------------------------------------------------
//
//  Microsoft Windows Media
//
//  Copyright (C) Microsoft Corporation, 1998 - 1998
//
//  File:       postfilter.cpp
//
//--------------------------------------------------------------------------
#include "bldsetup.h"

#include "xplatform.h"

#ifdef _SUPPORT_POST_FILTERS_
#include "limits.h"
#include "stdio.h"
#include "stdlib.h"
#include "wmvdec_member.h"
#include "typedef.hpp"
#include "postfilter_wmv.hpp"
#ifdef _WMV_TARGET_X86_
#include "cpudetect.h"
#endif
#include "opcodes.h"
#include "tables_wmv.h"

//THR1 = the threshold before we consider neighboring pixels to be "diffrent"
#define THR1 2       
//THR2 = the total number of "diffrent" pixels under which we use stronger filter
#define THR2 6
#define INV_THR2 3

Void_WMV (*g_pDeblockMB)(DEBLOCKMB_ARGS);
Void_WMV (*g_pApplySmoothing)(APPLYSMOOTHING_ARGS);
Void_WMV (*g_pDetermineThreshold)(DETERMINETHR_ARGS);
Void_WMV (*g_pDeringMB) (DERINGMB_ARGS);

#ifdef _Embedded_x86
#include "postfilter_emb.h"
#endif

Void_WMV g_InitPostFilter (Bool_WMV bFastDeblock) 
{
    g_pDeblockMB = DeblockMB;
    g_pApplySmoothing = ApplySmoothing;
    g_pDetermineThreshold = DetermineThreshold;
    g_pDeringMB = DeringMB;

#if defined(_WMV_TARGET_X86_) ||  defined(_Embedded_x86)
    if (g_SupportMMX ()) {        
        if (bFastDeblock)
            g_pDeblockMB = DeblockMB_FASTEST_MMX;
        else
            g_pDeblockMB = DeblockMB_MMX;
        g_pApplySmoothing = ApplySmoothing_MMX;
        g_pDetermineThreshold = DetermineThreshold_MMX;
    }
#ifdef _WMV_TARGET_X86_
    if (g_SupportSSE1()){
        g_pDetermineThreshold = DetermineThreshold_KNI;
        g_pDeblockMB = DeblockMB_KNI;
    }
#endif
#endif
}

inline I32_WMV MIN (I32_WMV a,I32_WMV b,I32_WMV c) 
{
    if (a < b) {
        if (a < c) return a;
        return c;
    } else {
        if (b < c) return b;
        else return c;
    }
}

inline I32_WMV CLIP (I32_WMV a,I32_WMV b,I32_WMV c) 
{
    if (b < c) {
        if (a < b) a = b;
        if (a > c) a = c;
        return a;
    }
    if (a > b) a = b;
    if (a < c) a = c;
    return a;
}

I32_WMV MAX(int a,int b,int c,int d,int e,int f,int g,int h) 
{
    I32_WMV max = a;
    if (max < b) max  = b;
    if (max < c) max  = c;
    if (max < d) max  = d;
    if (max < e) max  = e;
    if (max < f) max  = f;
    if (max < g) max  = g;
    if (max < h) max  = h;
    return max;
}

I32_WMV MIN(int a,int b,int c,int d,int e,int f,int g,int h) 
{
    I32_WMV min = a;
    if (min > b) min  = b;
    if (min > c) min  = c;
    if (min > d) min  = d;
    if (min > e) min  = e;
    if (min > f) min  = f;
    if (min > g) min  = g;
    if (min > h) min  = h;
    return min;
}

// this fn defined in spatialpredictor.cpp
Bool_WMV bMin_Max_LE_2QP(int a,int b,int c,int d,int e,int f,int g,int h, int i2Qp);

/*
#define P(m) (((m < 1) && (abs(v1-v0) < iStepSize)) ? v0 :   \
             (m <  1)                              ? v1 :    \
             (m == 1)                              ? v1 :    \
             (m == 2)                              ? v2 :    \
             (m == 3)                              ? v3 :    \
             (m == 4)                              ? v4 :    \
             (m == 5)                              ? v5 :    \
             (m == 6)                              ? v6 :    \
             (m == 7)                              ? v7 :    \
             (m == 8)                              ? v8 :    \
             (abs(v8-v9) < iStepSize)              ? v9 : v8)

#define filt(n) g_rgiClapTabDec[((P(n-4) + P(n-3) + 2*P(n-2) + 2*P(n-1) + 4*P(n) + 2*P(n+1) + 2*P(n+2) + P(n+3) + P(n+4) + 8)>>4)]
*/
#define P_filt1 rgiClapTab[((6*v0 + 4*v1 + 2*v2 + 2*v3 + v4 + v5 + 8)>>4)]
#define P_filt2 rgiClapTab[((4*v0 + 2*v1 + 4*v2 + 2*v3 + 2*v4 + v5 + v6 + 8)>>4)]
#define P_filt3 rgiClapTab[((2*v0 + 2*v1 + 2*v2 + 4*v3 + 2*v4 + 2*v5 + v6 + v7 + 8)>>4)]
#define P_filt4 rgiClapTab[((v0 + v1 + 2*v2 + 2*v3 + 4*v4 + 2*v5 + 2*v6 + v7 + v8 + 8)>>4)]
#define P_filt5 rgiClapTab[((v1 + v2 + 2*v3 + 2*v4 + 4*v5 + 2*v6 + 2*v7 + v8 + v9 + 8)>>4)]
#define P_filt6 rgiClapTab[((v2 + v3 + 2*v4 + 2*v5 + 4*v6 + 2*v7 + 2*v8 + 2*v9 + 8)>>4)]
#define P_filt7 rgiClapTab[((v3 + v4 + 2*v5 + 2*v6 + 4*v7 + 2*v8 + 4*v9 + 8)>>4)]
#define P_filt8 rgiClapTab[((v4 + v5 + 2*v6 + 2*v7 + 4*v8 + 6*v9 + 8)>>4)]

#define phi(a) ((abs(a) <= THR1) ? 1 : 0)     
#define SIGN(a) ((a < 0) ? -1 : 1)

#define inverse_phi(a) ((abs(a) > THR1) ? 1 : 0)     

inline Void_WMV FilterEdge (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* ppxlcCenter, I32_WMV iPixelDistance, I32_WMV iPixelIncrement, I32_WMV iEdgeLength, I32_WMV iStepSize
) {

    U8_WMV* pV5 = ppxlcCenter;
    U8_WMV* pV4 = pV5 - iPixelDistance;
    U8_WMV* pV3 = pV4 - iPixelDistance;
    U8_WMV* pV2 = pV3 - iPixelDistance;
    U8_WMV* pV1 = pV2 - iPixelDistance;
    U8_WMV* pV0 = pV1 - iPixelDistance;
    U8_WMV* pV6 = pV5 + iPixelDistance;
    U8_WMV* pV7 = pV6 + iPixelDistance;
    U8_WMV* pV8 = pV7 + iPixelDistance;
    U8_WMV* pV9 = pV8 + iPixelDistance;
    I32_WMV i;
    const U8_WMV* rgiClapTab = pWMVDec->m_rgiClapTabDec; 

    for (i = 0; i < iEdgeLength; ++i) {
       
       I32_WMV v0 = *pV0;                                                                    
       I32_WMV v1 = *pV1;                                                                    
       I32_WMV v2 = *pV2;                                                                    
       I32_WMV v3 = *pV3;                                                                    
       I32_WMV v4 = *pV4;                                                                    
       I32_WMV v5 = *pV5;                                                                    
       I32_WMV v6 = *pV6;                                                                    
       I32_WMV v7 = *pV7;                                                                    
       I32_WMV v8 = *pV8;                                                                    
       I32_WMV v9 = *pV9;                                                                    
       
       I32_WMV eq_cnt = phi(v0 - v1) + phi(v1 - v2) + phi(v2 - v3) + phi(v3 - v4) + phi(v4 - v5) +                     
                    phi(v5 - v6) + phi(v6 - v7) + phi(v7 - v8) + phi(v8 - v9);       
       
////////////// Run Experiement to see if it benefit.
       /*
       I32_WMV eq_cnt = inverse_phi(v2 - v3) + inverse_phi(v3 - v4) + inverse_phi(v4 - v5) +                     
                    inverse_phi(v5 - v6) + inverse_phi(v6 - v7);    
       if (eq_cnt <= INV_THR2)                                                          
           eq_cnt = (5 - eq_cnt_inv) + phi(v0 - v1) + phi(v1 - v2) + phi(v7 - v8) + phi(v8 - v9);       
       */ 

       if (eq_cnt >= THR2) {                                                         
           //I32_WMV max = MAX(v1,v2,v3,v4,v5,v6,v7,v8);    //DC Offset mode                               
           //I32_WMV min = MIN(v1,v2,v3,v4,v5,v6,v7,v8);                                   
           //if (abs(max-min) < 2*iStepSize){
           if (bMin_Max_LE_2QP(v1,v2,v3,v4,v5,v6,v7,v8,2*iStepSize)){                                          
               if (abs(v1-v0) >= iStepSize) v0 = v1;
               if (abs(v8-v9) >= iStepSize) v9 = v8;
               *pV1 = P_filt1;                                                   
               *pV2 = P_filt2;                                                        
               *pV3 = P_filt3;                                                        
               *pV4 = P_filt4;                                                        
               *pV5 = P_filt5;                                                        
               *pV6 = P_filt6;                                                        
               *pV7 = P_filt7;                                                        
               *pV8 = P_filt8;                                                        
/*
               *pV1 = filt(1);                                                   
               *pV2 = filt(2);                                                        
               *pV3 = filt(3);                                                        
               *pV4 = filt(4);                                                        
               *pV5 = filt(5);                                                        
               *pV6 = filt(6);                                                        
               *pV7 = filt(7);                                                        
               *pV8 = filt(8);                                                        
*/
           }                                                                          
       } else {
		   I32_WMV v4_v5 = v4 - v5;
		   I32_WMV a30 = (2*(v3-v6) - 5*v4_v5 + 4) / 8;
		   if (abs(a30) < iStepSize) {
			   I32_WMV v2_v3 = v2 - v3;
			   I32_WMV v6_v7 = v6 - v7;
			   I32_WMV a31 = (2*(v1-v4) - 5*v2_v3 + 4) / 8;                                 
			   I32_WMV a32 = (2*(v5-v8) - 5*v6_v7 + 4) / 8;                                 
			   I32_WMV iMina31_a32 = min(abs(a31),abs(a32)); 
			   if (iMina31_a32 < abs(a30)){
				   I32_WMV dA30 = SIGN(a30) * iMina31_a32 - a30;
				   I32_WMV d = CLIP(5*dA30/8,0,v4_v5/2);
				   *pV4 = rgiClapTab[v4 - d];                                                                     
				   *pV5 = rgiClapTab[v5 + d];
			   }
		   }
           /* 
           I32_WMV a30 = (2*v3 - 5*v4 + 5*v5 - 2*v6 + 4) >> 3;                                 
           I32_WMV a31 = (2*v1 - 5*v2 + 5*v3 - 2*v4 + 4) >> 3;                                 
           I32_WMV a32 = (2*v5 - 5*v6 + 5*v7 - 2*v8 + 4) >> 3;                                 
           I32_WMV A30 = SIGN(a30) * ( MIN(abs(a30),abs(a31),abs(a32))   );                
           I32_WMV d = CLIP( (5*(A30-a30)/8),0,((v4-v5)/2) * ((abs(a30) < iStepSize) ? 1 : 0)   );
           *pV4 = g_rgiClapTabDec[v4 - d];                                                                     
           *pV5 = g_rgiClapTabDec[v5 + d]; 
           */
       }
      
       pV0 += iPixelIncrement;
       pV1 += iPixelIncrement;
       pV2 += iPixelIncrement;
       pV3 += iPixelIncrement;
       pV4 += iPixelIncrement;
       pV5 += iPixelIncrement;
       pV6 += iPixelIncrement;
       pV7 += iPixelIncrement;
       pV8 += iPixelIncrement;
       pV9 += iPixelIncrement;
    }
} 

inline Void_WMV FilterHalfEdge (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* ppxlcCenter, I32_WMV iPixelDistance, I32_WMV iPixelIncrement, I32_WMV iEdgeLength, I32_WMV iStepSize)
{

    U8_WMV* pV5 = ppxlcCenter;
    U8_WMV* pV4 = pV5 - iPixelDistance;
    U8_WMV* pV3 = pV4 - iPixelDistance;
    U8_WMV* pV2 = pV3 - iPixelDistance;
    U8_WMV* pV1 = pV2 - iPixelDistance;
    U8_WMV* pV0 = pV1 - iPixelDistance;
    U8_WMV* pV6 = pV5 + iPixelDistance;
    U8_WMV* pV7 = pV6 + iPixelDistance;
    U8_WMV* pV8 = pV7 + iPixelDistance;
    U8_WMV* pV9 = pV8 + iPixelDistance;
    I32_WMV i;
    const U8_WMV* rgiClapTab = pWMVDec->m_rgiClapTabDec; 

    for (i = 0; i < iEdgeLength; ++i) {
       
       I32_WMV v0 = *pV0;                                                                    
       I32_WMV v1 = *pV1;                                                                    
       I32_WMV v2 = *pV2;                                                                    
       I32_WMV v3 = *pV3;                                                                    
       I32_WMV v4 = *pV4;                                                                    
       I32_WMV v5 = *pV5;                                                                    
       I32_WMV v6 = *pV6;                                                                    
       I32_WMV v7 = *pV7;                                                                    
       I32_WMV v8 = *pV8;                                                                    
       I32_WMV v9 = *pV9;                                                                    
                                                                                     
       I32_WMV eq_cnt = phi(v0 - v1) + phi(v1 - v2) + phi(v2 - v3) + phi(v3 - v4) + phi(v4 - v5) +                     
                    phi(v5 - v6) + phi(v6 - v7) + phi(v7 - v8) + phi(v8 - v9);       
                                                                                            
       if (eq_cnt >= THR2) {                                                         
           //I32_WMV max = MAX(v1,v2,v3,v4,v5,v6,v7,v8);    //DC Offset mode                               
           //I32_WMV min = MIN(v1,v2,v3,v4,v5,v6,v7,v8);                                   
           //if (abs(max-min) < 2*iStepSize){                                          
           if (bMin_Max_LE_2QP(v1,v2,v3,v4,v5,v6,v7,v8,2*iStepSize)){                                          
               if (abs(v1-v0) >= iStepSize) v0 = v1;
               *pV1 = P_filt1;                                                   
               *pV2 = P_filt2;                                                        
               *pV3 = P_filt3;                                                        
               *pV4 = P_filt4;                                                        
               /* 
               *pV1 = filt(1);                                                   
               *pV2 = filt(2);                                                        
               *pV3 = filt(3);                                                        
               *pV4 = filt(4);                                                        
               */
           }                                                                          
       } else {
		   I32_WMV v4_v5 = v4 - v5;
		   I32_WMV a30 = (2*(v3-v6) - 5*v4_v5 + 4) / 8;
		   if (abs(a30) < iStepSize) {
			   I32_WMV v2_v3 = v2 - v3;
			   I32_WMV v6_v7 = v6 - v7;
			   I32_WMV a31 = (2*(v1-v4) - 5*v2_v3 + 4) / 8;                                 
			   I32_WMV a32 = (2*(v5-v8) - 5*v6_v7 + 4) / 8;                                 
			   I32_WMV iMina31_a32 = min(abs(a31),abs(a32)); 
			   if (iMina31_a32 < abs(a30)){
				   I32_WMV dA30 = SIGN(a30) * iMina31_a32 - a30;
				   I32_WMV d = CLIP(5*dA30/8,0,v4_v5/2);
				   *pV4 = rgiClapTab [v4 - d];                                                                     
			   }
		   }
           /*
           I32_WMV a30 = (2*v3 - 5*v4 + 5*v5 - 2*v6 + 4) >> 3;                                 
           I32_WMV a31 = (2*v1 - 5*v2 + 5*v3 - 2*v4 + 4) >> 3;                                 
           I32_WMV a32 = (2*v5 - 5*v6 + 5*v7 - 2*v8 + 4) >> 3;                                 
                                                                                      
           I32_WMV A30 = SIGN(a30) * ( MIN(abs(a30),abs(a31),abs(a32))   );                
           I32_WMV d = CLIP( (5*(A30-a30)/8),0,((v4-v5)/2) * ((abs(a30) < iStepSize) ? 1 : 0)   );
           *pV4 = g_rgiClapTabDec[v4 - d];                                                                     
           */
       }
      
       pV0 += iPixelIncrement;
       pV1 += iPixelIncrement;
       pV2 += iPixelIncrement;
       pV3 += iPixelIncrement;
       pV4 += iPixelIncrement;
       pV5 += iPixelIncrement;
       pV6 += iPixelIncrement;
       pV7 += iPixelIncrement;
       pV8 += iPixelIncrement;
       pV9 += iPixelIncrement;
    
    }
}

//Find maxumum and minimum values in a 10x10 block
Void_WMV DetermineThreshold(U8_WMV *ptr, I32_WMV *thr, I32_WMV *range, I32_WMV width) 
{
    I32_WMV max = 0;                                         
    I32_WMV min = 255;  
    I32_WMV x, y;

    for (x = -1; x < 9; x++){                    
        for (y = -1; y < 9; y++){                
            I32_WMV pixelValue = ptr[x+(y*width)];       
            if (max < pixelValue) max = pixelValue;  
            if (min > pixelValue) min = pixelValue;  
        }                                            
    }
    *thr = (max + min + 1) / 2;
    *range = max - min;
}    

Void_WMV ApplySmoothing(U8_WMV *pixel,I32_WMV width, I32_WMV max_diff,I32_WMV thr)
{

    Bool_WMV bin[10][10];
    
    I32_WMV  x, y;
    U8_WMV output[8][8];

    for (x = 0; x < 10; x++){
        for (y = 0; y < 10; y++){
            U8_WMV pixelValue =  pixel[(x-1)+((y-1)*width)];
            bin [x][y] = (pixelValue >= thr) ? 1 : 0;
        }
    }


    for (x = 0; x < 8; x++){
        for (y = 0; y < 8; y++){
            if ((bin[x][y] == bin[x+1][y])    //If All 9 values equel (all 0 or all 1)
             && (bin[x][y] == bin[x+2][y])
             && (bin[x][y] == bin[x]  [y+1])
             && (bin[x][y] == bin[x+1][y+1])
             && (bin[x][y] == bin[x+2][y+1])
             && (bin[x][y] == bin[x]  [y+2])
             && (bin[x][y] == bin[x+1][y+2])
             && (bin[x][y] == bin[x+2][y+2])) {

                U8_WMV *ppxlcFilt = pixel+x+(y*width);

                //Apply Spoothing Filter
                I32_WMV filt = (  ppxlcFilt[-1-width] +  2*ppxlcFilt[0 -width] +   ppxlcFilt[+1-width] +
                            2*ppxlcFilt[-1      ] +  4*ppxlcFilt[0       ] + 2*ppxlcFilt[+1      ] +      
                              ppxlcFilt[-1+width] +  2*ppxlcFilt[0 +width] +   ppxlcFilt[+1+width] +8) >> 4;
                
                if ((filt - *ppxlcFilt) > max_diff) filt = *ppxlcFilt + max_diff;
                else if ((filt - *ppxlcFilt) < -max_diff) filt = *ppxlcFilt - max_diff;
                CLIP(filt,0,255);
                output[x][y] = (U8_WMV)filt;

            } else output[x][y] = *(pixel+x+(y*width));
        }
    }
    for (x = 0; x < 8; x++){
        for (y = 0; y < 8; y++){
        *(pixel+x+(y*width)) = output[x][y];
           
        }
    }
}

Void_WMV DeblockMB (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV        *ppxliY,
    U8_WMV        *ppxliU,
    U8_WMV        *ppxliV,
    Bool_WMV                  bDoLeft,
    Bool_WMV                  bDoRightHalfEdge,
    Bool_WMV                  bDoTop,
    Bool_WMV                  bDoBottomHalfEdge,
    Bool_WMV                  bDoMiddle,
    I32_WMV                   iStepSize,
    I32_WMV                   iWidthPrevY,
    I32_WMV                   iWidthPrevUV
)
{
    
    if (bDoTop) {
        // Filter the top Y, U and V edges.
        FilterEdge (pWMVDec, ppxliY,iWidthPrevY,1,MB_SIZE,iStepSize);
        FilterEdge (pWMVDec, ppxliU,iWidthPrevUV,1,BLOCK_SIZE,iStepSize);
        FilterEdge (pWMVDec, ppxliV,iWidthPrevUV,1,BLOCK_SIZE,iStepSize);
    }

    if (bDoBottomHalfEdge) {
        // Filter the bottom Y, U and V edges.
        FilterHalfEdge (pWMVDec, ppxliY + (iWidthPrevY * MB_SIZE),iWidthPrevY,1,MB_SIZE,iStepSize);
        FilterHalfEdge (pWMVDec, ppxliU + (iWidthPrevUV * BLOCK_SIZE),iWidthPrevUV,1,BLOCK_SIZE,iStepSize);
        FilterHalfEdge (pWMVDec, ppxliV + (iWidthPrevUV * BLOCK_SIZE),iWidthPrevUV,1,BLOCK_SIZE,iStepSize);
    } 

    if (bDoMiddle) {
        // Filter the middle horizontal Y edge
        FilterEdge(pWMVDec, ppxliY+ iWidthPrevY * 8,iWidthPrevY,1,MB_SIZE,iStepSize);

        // Filter the middle vertical Y edge
        FilterEdge(pWMVDec, ppxliY + 8,1,iWidthPrevY,MB_SIZE,iStepSize);     
    }

    if (bDoLeft) {
        // Filter the left vertical Y, U and V edges.
        FilterEdge(pWMVDec, ppxliY,1,iWidthPrevY,MB_SIZE,iStepSize);
        FilterEdge(pWMVDec, ppxliU,1,iWidthPrevUV,BLOCK_SIZE,iStepSize);
        FilterEdge(pWMVDec, ppxliV,1,iWidthPrevUV,BLOCK_SIZE,iStepSize);
    }

    if (bDoRightHalfEdge) {
        // Filter the right vertical Y, U and V edges.
        FilterHalfEdge(pWMVDec, ppxliY + MB_SIZE,1,iWidthPrevY,MB_SIZE,iStepSize);
        FilterHalfEdge(pWMVDec, ppxliU + BLOCK_SIZE,1,iWidthPrevUV,BLOCK_SIZE,iStepSize);
        FilterHalfEdge(pWMVDec, ppxliV + BLOCK_SIZE,1,iWidthPrevUV,BLOCK_SIZE,iStepSize);
  } 
}

Void_WMV DeringMB (
    U8_WMV        *ppxlcY,
    U8_WMV        *ppxlcU,
    U8_WMV        *ppxlcV,
    I32_WMV                  iStepSize,
    I32_WMV                  iWidthPrevY,
    I32_WMV                  iWidthPrevUV
)
{  
    I32_WMV thr[6];
    I32_WMV range[6];
    I32_WMV k_max;
    I32_WMV max_range;
    I32_WMV k;
    
    g_pDetermineThreshold(ppxlcY,                                    &thr[0],&range[0],iWidthPrevY);
    g_pDetermineThreshold(ppxlcY+ BLOCK_SIZE,                        &thr[1],&range[1],iWidthPrevY);
    g_pDetermineThreshold(ppxlcY+(BLOCK_SIZE*iWidthPrevY),           &thr[2],&range[2],iWidthPrevY);
    g_pDetermineThreshold(ppxlcY+(BLOCK_SIZE*iWidthPrevY)+BLOCK_SIZE,&thr[3],&range[3],iWidthPrevY);
    g_pDetermineThreshold(ppxlcU,                                    &thr[4],&range[4],iWidthPrevUV);
    g_pDetermineThreshold(ppxlcV,                                    &thr[5],&range[5],iWidthPrevUV);
    
    k_max = (range[0]     > range[1]) ? 0     : 1;
    k_max = (range[k_max] > range[2]) ? k_max : 2;
    k_max = (range[k_max] > range[3]) ? k_max : 3;
    max_range = range[k_max];
    for (k = 0; k < 4; k++){
        if ((range[k] < 32) && (max_range >= 64)) thr[k] = thr[k_max];
        if (max_range < 16) thr[k] = 0;
    }

    g_pApplySmoothing(ppxlcY                                    ,iWidthPrevY ,iStepSize*2,thr[0]);
    g_pApplySmoothing(ppxlcY+BLOCK_SIZE                         ,iWidthPrevY ,iStepSize*2,thr[1]);
    g_pApplySmoothing(ppxlcY+(BLOCK_SIZE*iWidthPrevY)           ,iWidthPrevY ,iStepSize*2,thr[2]);
    g_pApplySmoothing(ppxlcY+BLOCK_SIZE+(BLOCK_SIZE*iWidthPrevY),iWidthPrevY ,iStepSize*2,thr[3]);
    g_pApplySmoothing(ppxlcU                                    ,iWidthPrevUV,iStepSize*2,thr[4]);
    g_pApplySmoothing(ppxlcV                                    ,iWidthPrevUV,iStepSize*2,thr[5]);
}

#endif // _SUPPORT_POST_FILTERS_
