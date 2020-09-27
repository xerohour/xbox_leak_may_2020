#ifndef _CORRPDA_H_
#define _CORRPDA_H_
/***********************************************************************
 * 
 * Filename:  CorrPDA.h
 *
 * Purpose:   Calculate cross-correlation related pitch esti-
 *            mator. Then backward / forward track the pitch.
 *           
 * Reference: (1) "A 5.85kb/s CELP Algorithm For Cellular Application."
 *                 W. Kleijn etc, ICASSP 1993, p.596.
 *            (2) Email from W. Kleijn to B. McAulay, 1995.
 *            (3) ITU-T Draft G.723, p.13.
 *
 * Author:    Wei Wang
 *
 * Date:      Oct. 4, 1996
 *
 * History:
 *            Rewrite the code for decimated signal.
 *
 * $Header:   /export/phobos/pvcs/r_and_d/archives/realtime/pda/CorrPDA.h_v   1.5   04 Dec 1998 16:24:50   weiwang  $
 ***********************************************************************/


/**************************************************
 * structure for cross-correlation PDA.
 **************************************************/

#define USE_HPF    1     /* if 1: use HPF */
                         /*    0: no HPF -- good for fixed-point implementation */

typedef struct
{
  /****** inter-frame memory -- total (252) ******/
  /****** if considering the PDA delay and circular buffer and input speech ((261-180)/2+2*180=401)
    then the total inter-frame memory will be (653) *******/
#if USE_HPF == 1
  float *hpf_mem;         /* memory for high-pass filter (4) */
#endif

  float *lpf_mem;         /* memory for low-pass filter (3) */

  float *lpfbuf;          /* decimated signal buffer (90+45) */
  
  float *costs0;          /* normalized costs value of previous frame (90-5+1=86)*/

  float *Alsf0;           /* LSF of previous frame -- for interpolation (10) */

  float *percWeight_mem;  /* memory for perceptual weighting filter (10) */

  int pitch_1;            /* pitch of previous frame (1) */

  int pitch_2;            /* pitch of previous-previous frame (1) */

  float cost_1;           /* cost value of previous frame (1) */

  float cost_2;           /* cost value of previous-previous frame (1) */
  

  /****** tables: pre-calculated for general application
    or load-in to ROM for DSP application *******/
  /****** total :  (626) *******/
  float *lpfCoefs;        /* (3+4 = 7) */

#if USE_HPF == 1
  float *hpfCoefs;        /* (4+5 = 9) */
#endif

  float *LpcWindow;       /* asymmetric window for LPC calculation (180) */

  float *biasTab;         /* table for biasing the normalized costs (86) */

  int *dInterLagStart;    /* for interframe tracking: starting lags (86) */

  int *dInterLagLength;   /* for interframe tracking: tracking length (86) */

  int *dIntraLagStart;    /* for intraframe tracking: starting lags (86) */

  int *dIntraLagLength;   /* for intraframe tracking: tracking length (86) */


  /****** For scratch memories: (re-usable) *******/
  /****** total: 589 *******/
  void *myLPC2LSP_mblk;    /* for lpc to lsp. Will take out this structure after changing lpctolsp.c */

  void *myDurbin_mblk;     /* for calcDurbin. Will take out this structure after using new stack memory strategy */

  void *VADMemBlk;         /* Memory required by VAD.                         */

  /****** others need reuse tmpSigBuf, or tmpSubCosts or tmpEnergyRef ********/
} CorrPDA_mblk;

/*************************************************************
 *
 * Function: VoxCorrPDA()
 *
 * Get input speech from circular buffer and then apply corrPitchEst().
 *
 * Wei Wang.   05/05/97
 *************************************************************/
float VoxCorrPDA (STACK_R float *speechIn, short framelen, 
		void *pvCorrPDA_mblk, float *pda_delay_size, 
                short *VAD,short *UV);



/***********************************************************************
 * Function: initCorrPitchEst()
 *
 * Input:    myCorrPDA_mblk:  pointer for the structure.
 *
 * Output:   error code:   error with initialization.
 *           0:            no error
 ***********************************************************************/

unsigned short VoxInitCorrPitchEst(void **myCorrPDA_mblk);



/***********************************************************************
 * Function: freeCorrPitchEst()
 *
 * Input:    myCorrPDA_mblk:   pointer for the structure.
 *
 * Output:   None
 ***********************************************************************/

unsigned short VoxFreeCorrPitchEst(void **myCorrPDA_mblk);

float PvRefine( float Pv, float Pv_1, float Pitch, float MaxCost, float *biasTab);

#endif /* end of #ifndef _CORRPDA_H_ */



