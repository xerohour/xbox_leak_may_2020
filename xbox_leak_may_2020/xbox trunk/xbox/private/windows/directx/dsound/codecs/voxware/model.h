/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/

/*******************************************************************************
* Filename:       model.h
*                                                                              
* Purpose:        Provide definitions pertaining to the model codec ONLY.
*                                                                              
* Author/Date:    Rob Zopf  02/17/97
********************************************************************************
* $Header:   /export/phobos/pvcs/r_and_d/archives/realtime/include/model.h_v   1.4   01 Dec 1998 14:37:48   zopf  $                                                                      
*******************************************************************************/

#ifndef _MODEL_H
#define _MODEL_H

#include "lpcorder.h"
/*-----------------------------------------------------------------------------
    C U R R E N T L Y   S U P P O R T E D   C O M P I L E   S W I T C H E S 
 -----------------------------------------------------------------------------*/
#define FULL_BAND_PV                1   /* 1: using whole band for voicing decision 0:3984.375Hz */

/*-----------------------------------------------------------------------------
            G E N E R A L   C O D E C   D E F I N I T I O N S
 -----------------------------------------------------------------------------*/
#define FRAME_LEN                   180
#define MAXFRAMELEN                 180
#define RATE8K                      8000.0F
#define INV_RATE8K                  (1.0F/RATE8K) /* RTE */
#undef PI
#define PI                          (double)3.141592654
#define D_PI                        (2.0*PI)


#if FULL_BAND_PV == 0
#define ARBITRARY_CUTOFF            0.9254F
#else
#define ARBITRARY_CUTOFF           0.99609375F /* (255/256) using whole band 0:3984.375Hz */
#endif

/*------------------------------------------------------------------------------
          E N C O D E R   S P E C I F I C   D E F I N I T I O N S 
------------------------------------------------------------------------------*/

#define PRE_PROC_COEF               0.99F

#define MIN_PITCH                   8
#define MAX_PITCH                   180
#define CIRCBUF_LENGTH              401

#define FRAME_SIZE1                 221
#define FRAME_SIZE2                 241
#define FRAME_SIZE3                 261

#define PITCH4FRAME_SIZE2           60.0F   
#define PITCH4FRAME_SIZE3           43.0F   

#define EXPAND_COEF                 0.994F

#define MAXHARM                     (MAX_PITCH>>1)
#define MAXHARM_2                   (MAXHARM>>1)

#define MAX_F0                      (RATE8K/MIN_PITCH)

#define PITCH_UV                  125     /* 64 Hz */

/* Encoder FFT Lengths */
#define FFT_ORDER                   9   /* change this to something more descriptive */
#define NFFT                        (1<<FFT_ORDER)
#define NFFT_2                      (NFFT>>1)
#define NFFT_4                      (NFFT>>2)

/*------------------------------------------------------------------------------
          D E C O D E R   S P E C I F I C   D E F I N I T I O N S 
------------------------------------------------------------------------------*/
#define LPCSYN_SUBFRAMES            4
#define SYNSUBFRAMES                3
#define LPC_ORDER                   CODEC_LPC_ORDER    
#define OVERLAPFACTOR               1.0F
#define PITCH_RANGE_FOR_INTERP      0.15F
#define MAXSUBFRAMELEN             (short)(FRAME_LEN/SYNSUBFRAMES)
#define MAXSYNLEN                   MAXSUBFRAMELEN*2-1
#define ENHANCE_FFT_REAL_ORDER      6
#define LSP_SEPERATION_HZ           45.0F
#define MAX_PITCH_MULT              125

#define LSP_INITIAL_COND_0			300.0F 	/*%# 300Hz -- RTE */
#define LSP_INITIAL_COND_DEL        (3600.0F/(float)(LPC_ORDER-1)) /* RTE */
#define SPEECH_CLIP_MAX        		32000   
#define SPEECH_CLIP_MIN        		-32000  

#define USE_FHT_SYN                 1

#if (MAXHARM < 64)
#define EXP_SHIFT                   7
#elif (MAXHARM < 128)
#define EXP_SHIFT                   8
#endif
#if (MAXHARM >=128)
#error                              "MAXHARM Exceeds FHT Range"
#endif
#define EXP_SIZE                    (1<<EXP_SHIFT) 

#define QPHASE                      10




#define BARK_PW_BANDS 				20	 /* # of perceptual weighting bands for Amplitude quantization */
										 /* this must match to quantizer !!! */

/*----------------------------------------------------
  BackGround Noise Smoothing
  ----------------------------------------------------*/
#define HANGOVER_TIME 10
#define AVE_FACTOR  0.2F


#endif

