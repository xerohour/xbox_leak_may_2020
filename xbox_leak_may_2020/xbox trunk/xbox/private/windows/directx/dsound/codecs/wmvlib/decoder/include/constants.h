/*************************************************************************

Copyright (C) Microsoft Corporation, 1996 - 1999

Module Name:

    Constants.h

Abstract:

    Contants and macros used in MsAudio.

Author:

    Wei-ge Chen (wchen) 12-March-1998

Revision History:


*************************************************************************/

#ifndef __CONSTANTS_H_
#define __CONSTANTS_H_

#ifdef  PI
#undef  PI
#endif
#define PI  3.1415926535897932384626433832795
// value prior to 10/5/99 as a float: #define PI  3.14159265359F

#define FLOAT_SHRT_MAX  32767.0F - 10.0F
#define FLOAT_SHRT_MIN -32768.0F + 10.0F
#define FLOAT_INT_MAX  2147483647.0F - 10.0F
#define FLOAT_INT_MIN -2147483648.0F + 10.0F

#ifdef	BITS_PER_BYTE
#undef	BITS_PER_BYTE
#endif
#define BITS_PER_BYTE 8

#ifdef	BITS_PER_DWORD
#undef	BITS_PER_DWORD
#endif
#define BITS_PER_DWORD      32

#ifdef	BITS_PER_QWORD
#undef	BITS_PER_QWORD
#endif
#define BITS_PER_QWORD      64

#ifdef	NBITS_PACKET_CNT
#undef	NBITS_PACKET_CNT
#endif
#define NBITS_PACKET_CNT    4

#ifdef	NBITS_FRM_CNT
#undef	NBITS_FRM_CNT
#endif
#define NBITS_FRM_CNT       4


#ifdef  NUM_MILSEC_PER_FRAME
#undef  NUM_MILSEC_PER_FRAME
#endif
#define NUM_MILSEC_PER_FRAME 32

#ifdef  NUM_BARK_BAND
#undef  NUM_BARK_BAND
#endif
#define NUM_BARK_BAND 25

#ifdef  MAX_WIDTH_BARKBAND
#undef  MAX_WIDTH_BARKBAND
#endif
#define MAX_WIDTH_BARKBAND 450

#ifdef  MIN_BITRATE
#undef  MIN_BITRATE
#endif
#define MIN_BITRATE 2000    //2.0kbps, TBD

#ifdef  MAX_BITRATE
#undef  MAX_BITRATE
#endif
#define MAX_BITRATE 640000  //64kbps, TBD

#ifdef  LOW_CUTOFF_V1
#undef  LOW_CUTOFF_V1
#endif
#define LOW_CUTOFF_V1 3        //TBD

#ifdef  LOW_CUTOFF
#undef  LOW_CUTOFF
#endif
#define LOW_CUTOFF 0        //wchen: no more due to bug#19144

#ifdef  ABS_THRESHOLD_IN_DB     // in dB, how much to bring down Fletcher-Munson curves;
#undef  ABS_THRESHOLD_IN_DB
#endif
#define ABS_THRESHOLD_IN_DB -30

#define PWRTODB_STEP        ((Float)2.5)
#define PWRTODB_NOSCALE     24 // 61/0.4, Calculated using B=608 in 10*log10(0xFFFFFFFF*2^0/B) - 7
#define PWRTODB_SIZE        57 // 143.5/0.4, Calculated using B=1 in 10*10log10(0xFFFFFFFF*2^18/B) - 7
#define PWRTODB_FULLSCALE   20 // We chop off 18 whole bits (+ any fraction bits) in full-scale pwr-to-db tbl

// Number of bits to discard when calculating power (max power > U32, need to make room)
#define PWRTODB_DEFSACRIFICE    (PWRTODB_FULLSCALE + 2*TRANSFORM_FRACT_BITS)

// power reduction factor in dB for masking @ same frequency
#ifdef INTEGER_ENCODER
#define MIN_THRESHOLD_IN_DB_V1  ((I32)(25 / PWRTODB_STEP ))
#else   // INTEGER_ENCODER
#define MIN_THRESHOLD_IN_DB_V1  25
#endif  // INTEGER_ENCODER

#ifdef  MIN_MASKQ_IN_DB_V1
#undef  MIN_MASKQ_IN_DB_V1
#endif
#define MIN_MASKQ_IN_DB_V1 (Int) (MIN_THRESHOLD_IN_DB_V1 / MASK_QUANT)

#ifdef  MAX_MASKQ_IN_DB_V1
#undef  MAX_MASKQ_IN_DB_V1
#endif                                      
#define MAX_MASKQ_IN_DB_V1 MIN_MASKQ_IN_DB_V1 + (1 << NBITS_FIRST_MASKQUANT) - 1

#ifdef  MASK_QUANT      // in db
#undef  MASK_QUANT      
#endif
#define MASK_QUANT 2.5

#ifdef  NBITS_FIRST_MASKQUANT      
#undef  NBITS_FIRST_MASKQUANT      
#endif
#define NBITS_FIRST_MASKQUANT 5   

#ifdef  FIRST_V2_MASKQUANT
#undef  FIRST_V2_MASKQUANT
#endif
#define FIRST_V2_MASKQUANT 36

#ifdef  MAX_MASKQUANT_DIFF      
#undef  MAX_MASKQUANT_DIFF      
#endif
#define MAX_MASKQUANT_DIFF 60  // limited by the huffman table

#ifdef  NUM_BITS_QUANTIZER
#undef  NUM_BITS_QUANTIZER
#endif
#define NUM_BITS_QUANTIZER 7

// both as the escape code and the range...
#ifdef  MSA_MAX_QUANT_ESC
#undef  MSA_MAX_QUANT_ESC
#endif
#define MSA_MAX_QUANT_ESC ((1<<NUM_BITS_QUANTIZER) - 1)

#ifdef  MAX_QUANT
#undef  MAX_QUANT
#endif
#define MAX_QUANT 129	/* 138 has been seen by decoder */

#ifdef  MIN_QUANT
#undef  MIN_QUANT
#endif
#define MIN_QUANT 1		/*	was once 10; 22 is lowest value seen by decoder */

#define MAX_RUNLEVEL_44SM_QB 60
#define MAX_RUNLEVEL_44SS_QB 40
#define MAX_RUNLEVEL_44SM_HB 110
#define MAX_RUNLEVEL_44SS_HB 70
#define MAX_RUNLEVEL_44SM_OB 340
#define MAX_RUNLEVEL_44SS_OB 180
#define MAX_RUNLEVEL_16SM_OB 70
#define MAX_RUNLEVEL_16SS_OB 40

//lpc constants;need clean up
#define LPCORDER 10
#define FOURTYFOURS_QB 1
#define FOURTYFOURS_OB 2
#define SIXTEENS_OB 3

#define MIN_AUDIBLE 0   // should be fixed...

#define MSA_STEREO_WEIGHT 1.6

#define MIN_EQ -35
#define MAX_EQ 15
#define MIN_RATE BITS_PER_BYTE

#define NUM_PREV_POWER 8
#define NUM_TRANS_FILTER 3

// We're lucky in that everywhere where MIN_FRAME_SIZE is used, pau->m_fHalfTransform
// is available. If this changes, the compiler will let you know.
#define MIN_FRAME_SIZE  HALF(pau->m_fHalfTransform, 128)

// Constants
#define QUANTSTEP_DENOMINATOR 20
#define QUANTSTEP_FRACT_BITS 28

#define TRANSFORM_FRACT_BITS    5

#define MAX_CSUBBAND 2048

// The following should only be used when compiling the integer encoder
#ifdef INTEGER_ENCODER
#define INVWEIGHTFACTOR_FRACTBITS   27
#define INVQUANTSTEP_FRACTBITS      27
#define WEIGHTINPUT_FRACTBITS   14  // prvWeightInput
#define WEIGHT_FRACTBITS        4   // prvGetMaskingThresholdMono and others in masking.c

// Discard bits to allow power value to stay within U32 range. We only need to discard
// 3 bits but discard 1 extra so sqrt(2^TRANSIENT_BITDISCARD) is power of 2
#define TRANSIENT_THRFRACTBITS  4       // Allows 1/16 precision (see threshold init)
#define TRANSIENT_BITDISCARD    (3 + 1 + TRANSIENT_THRFRACTBITS)
#endif  // INTEGER_ENCODER


// fix-point binary points conversion factors
// convert float or double to BP1 integer ( -1.0 <= x < +1.0 )
#define NF2BP1 0x7FFFFFFF
// convert float or double to BP2 integer ( -2.0 <= x < +2.0 )
#define NF2BP2 0x3FFFFFFF

//wchen: added for new buffer model 

#define NOT_DETECTED_YET -2
#define NO_TRANSIENT     -1

#endif //__CONSTANTS_H_
