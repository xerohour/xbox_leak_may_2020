/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
******************************************************************************/

/*******************************************************************************
* Filename:       Codec.h
*                                                                              
* Purpose:        Provides global defines for entire codec
*                                                                              
* Author/Date:    Ilan Berci  96/10/25
********************************************************************************
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/include/codec.h_v   1.10   13 Apr 1998 18:26:22   weiwang  $                                                                     
******************************************************************************/

/* The objective is to keep this file as SMALL AS POSSIBLE!!!! */

#ifndef _CODEC_H
#define _CODEC_H

#undef PI

#ifndef PI
#define PI           3.1415926535897931032F
#define TWOPI        6.2831853071795862064F
#define INVTWOPI     0.1591549430918953456F
#define PIO2         1.5707963267948965580F
#endif /* PI */

#define D_PI         (double)(PI*(double)2.0)
#define D_PI_PERIOD  (double)(D_PI/(double)8000)

/***** Frame length...80 samples in 10ms sub-frame *****/
#define HALF_FRAME_LENGTH_SC   80
#define FRAME_LENGTH_SC        (HALF_FRAME_LENGTH_SC<<1)

/***** DATA length for coarse pitch analysis *****/
#define PDA_FRAME_SIZE0        291   /** was 261**/
#define INV_PEA_FRAME_SIZE0    (1.0F/(float)PDA_FRAME_SIZE0)

/***** DATA length for pitch-adaptive analysis *****/

#define PDA_FRAME_SIZE1        261   /** 32.5ms **/
#define PDA_FRAME_SIZE2        241   /** 30.0ms **/
#define PDA_FRAME_SIZE3        221   /** 27.5ms **/
#define PDA_FRAME_SIZE4        201   /** 25.0ms **/
#define PDA_FRAME_SIZE5        181   /** 22.5ms **/
#define PDA_FRAME_SIZE6        161   /** 20.0ms **/
#define PDA_FRAME_SIZE7        161   /** 20.0ms **/
#define PDA_FRAME_SIZE8        161   /** 20.0ms **/
#define PDA_FRAME_SIZE9        161   /** 20.0ms **/
#define PDA_FRAME_SIZE10       161   /** 20.0ms **/


/* The maximum number of harmonics is 4000*0.9275/64 = 58 */
#define MAXHARM            100    
#define MAXHARM_2          50


/*** Pitch period range in samples ***/
#define MIN_PITCH      8      /** 500Hz...was 16 (500Hz) **/
#define MAX_PITCH      160     /** 64Hz...was 165 (50Hz) **/


/*** Initial Pitch in DFT points ***/
#define INITIAL_PITCH     100.0F
#define INITIAL_PITCH_DFT 5    /** ~75Hz using 512-pt FFT and 8000Hz sampling */


/*** Analysis FFT parameters ***/
#define NFFT              512
#define NFFT_2            256
#define NFFT_4            128
#define FFT_ORDER           9

#define INV_NFFT        (1.0F/(float)NFFT)

#define RATE8K           8000   /* sampling rate */
#define HZ2DFT           ((float)NFFT/(float)RATE8K)
#define TWOPIONFFT       (TWOPI/(float)NFFT)

/*** Define number of measured phases for 6K bit rate ***/
#define SC6_PHASES          8

 
/***** Spectral parameters */
#define LPC_ORDER         12  /* LPC order for onset estimation */

#define ORDER_CEPSTRAL    44   

#define ASK_RES_ORDER      2

/***** preprocessing and circular buffer parameters *****/
#define PREPROC_COEF         0.99F

/* to be analysed */
#define CIRCBUF_LENGTH       (PDA_FRAME_SIZE0+HALF_FRAME_LENGTH_SC) 



#define CSENVDWNSMP         2  /* amount to downsample cs2env      */
#define CSENV_FFT_ORDER    (FFT_ORDER-CSENVDWNSMP)
#define CSENV_NFFT         (1<<CSENV_FFT_ORDER)
#define CSENV_NFFT_2       (CSENV_NFFT>>1)


/*-------------------------------------------------------
  Definitions for FHT 
  -------------------------------------------------------*/
#define FHT_EXP_SHIFT     8
#define FHT_EXP_SIZE      (1<<FHT_EXP_SHIFT)


/*------------------------------------------------------------
  project related defintions 
  ------------------------------------------------------------*/
/***** delay related definitions for LM project *****/
#define ENC_DELAY_SIZE  (PDA_FRAME_SIZE0>>1)  /****** the encoder delay ******/

/***** definition related to pitch refinement (for both RefineC0.c and MidPitch.c) *****/
#define MAX_DELTA   4            /***** maximum time-domain pitch refinement range ****/

#define   SRANGE1      0.75F
#define   FRACTION1    0.375F

#define   SRANGE2      0.25F 
#define   FRACTION2    0.125F


#define EPS 1e-16F

#define F0_SHIFT_BITS               12
#define SCALED_05                   ((long)1<<(F0_SHIFT_BITS-1))

#define COMPRESS_RATIO_MAG           0.125F   /* 0.25/2 */

#define POLY_ORDER_p1  6
#define N_REGION       6

#define   F0_SHIFT_11B          11
#define VF_CEP_ORDER         12
#define INV_ORDER            (1.0F/(float)VF_CEP_ORDER)
#define VF_BAD_PITCH         1000.0F

#define NOISE_LEVEL  0.001F  /** add noise to the coarse pitch FFT for stability...was 1.0 **/
#define NOISE_FLOOR     0.00001F     /* reject STFT peaks 50dB down */

#define PARAM_MIDFRAME     0
#define PARAM_ENDFRAME     1

#endif /* _CODEC_H */

