#ifndef _PITCHDEF_H_
#define _PITCHDEF_H_

/*************************************************************
 * 
 * Filename:   PitchDef.h
 *
 * Purpose:    Definitions for pitch estimation algorithm.
 *
 * Author:     Wei Wang
 *
 * Date:       Oct. 4, 1996
 *
 * $Header:   /export/phobos/pvcs/r_and_d/archives/realtime/pda/PitchDef.h_v   1.4   12 May 1998 12:55:10   weiwang  $
 *************************************************************/

#include "lpcorder.h"

#define FRAME_LEN          180
#define RATE8K                      8000.0F
#define INV_RATE8K                  (1.0F/RATE8K) /* RTE */

#define PITCH_MIN          10
#define PITCH_MAX          180 

#define NUM_SUBFRAME       3

#define SUBFRAME_LEN      (FRAME_LEN/NUM_SUBFRAME)

#define PDA_EXPAND_COEF             0.9921875F    /* == (254/256) */

#define PERCEP_WEIGHT_NUM_BIG       0.9921875F    /* == (254/256) */
#define PERCEP_WEIGHT_NUM_SMALL     0.98828125F   /* == (253/256) */
#define PERCEP_WEIGHT_DEN           0.8F

#define MIN_DIFF_LSF                64.0F    /* 64.0Hz if Lsfs is normalized by 8000.0Hz */

#define INTERP_LSF_COEF          ((float)1.0F/(float)NUM_SUBFRAME)

#define HPF_ORDER                   4

#define LPF_ORDER                   3

#define PDA_LPC_ORDER               10

#define LEFT_ASYM_WIN               135
#define RIGHT_ASYM_WIN              (FRAME_LEN-LEFT_ASYM_WIN)

#define HAMMING_CONST               0.54F

/* decimated domain */
#define LFRAME_LEN         (FRAME_LEN/2)
#define LFRAME_LEN_2       (LFRAME_LEN/2)

#define LPITCH_MIN         (PITCH_MIN/2)
#define LPITCH_MAX         (PITCH_MAX/2)

#define LSUBFRAME_LEN     (SUBFRAME_LEN/2)

#define PITCH_CAND         (LPITCH_MAX-LPITCH_MIN+1)

#define INIT_PITCH          40
#define INIT_COST           -1.0F

#define INTRA_FRAME_TRACK_RATIO    0.155F
#define INTER_FRAME_TRACK_RATIO    0.25F
#define CENTER_TRACK_RATIO         0.1F

#define LAST_SUBFRAME_WEIGHT       0.5F


#define BIAS_BACKWARD         0.93F            /* was 0.9166666667  */

#define BIAS_FORWARD          0.98F           /* was 0.95  */

#define BIAS_C0               0.27777777777778F  /* (1/3.6) (0.2631578947368  ==>  1/3.8)  */

#define NOISE_FLOOR            0.15F

#define INV_NOISE_FLR          1.17647059F

#define LOW_ENERGY_RATIO        0.25F
#define SQRT_LOW_ENERGY_RATIO   0.5F

#define CENT_WIN_A              0.75F
#define CENT_WIN_B              0.25F

#define MINCON                  1.0e-6F
#define MAXFLT                  3.402823466E+38F  

#ifndef M_PI
#define M_PI                       (double)3.14159265358979323846
#endif

#ifndef ZERO_LSF                   
#define ZERO_LSF                   363.6F
#endif

#ifndef LSP_INITIAL_COND_0                   
#define LSP_INITIAL_COND_0			300.0F 	/*%# 300Hz #%*/
#define LSP_INITIAL_COND_DEL        (3600.0F/(float)(PDA_LPC_ORDER-1))
#endif


#define LPITCH_CONF                 20 

#define LPITCH_MIN_CONF             10    /* if pitch == 20, we feel more confidence */

#define PDA_WIN_COEF                0.58F            /* was 64F */
#define PDA_WIN_DIFFC               ((1.0F-PDA_WIN_COEF)/(float)LFRAME_LEN)

#endif /* end of #ifndef _PITCHDEF_H_ */


