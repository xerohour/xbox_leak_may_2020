/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1997, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*                                                                              *
*******************************************************************************/
 
/*******************************************************************************
*
* File:          vLibFilt.h
*
* Purpose:
*
* Functions:
*
* Author/Date:
*******************************************************************************
* Modifications:
*
* Comments:
*
* Concerns:
*
* $Header:   /export/phobos/pvcs/pvcsproj/voxlib.prj/vlibfilt.h_v   1.23   26 Feb 1998 15:30:50   weiwang  $
******************************************************************************/
 
#ifndef _VLIB_FILT
#define _VLIB_FILT

#ifdef __cplusplus
extern "C" {
#endif


/* PolyIntp.c */
/* The definitions need for application:  */
/*****************************************************************************
*  PP_NUM_SAMPLES: number of samples for polyphase FIR filter.
*  PP_DELAY      : delay in samples caused by polyphase FIR filter
******************************************************************************/
#define PP_NUM_SAMPLES                    4
#define PP_DELAY                          ((PP_NUM_SAMPLES>>1)-1)

/******************************************************************************
*
* Function:  PolyCycleIntp()
*
* Action:    Poly-Phase Cycle Interpolation.
*
* Input:    pfOut       - address to the resampling signal.
*           iOutLength  - the length of the resampling signal.
*           pfCycle     - input proto-type signal, usually refer to a single
*                         pitch epoch with 0 phase.
*           iLog2CycleLength 
*                       - log2 of the input signal length. 
*           fResampRate - the resampling ratio of the input cycle with
*                         the output cycle (calculated by input cycle length 
*                         over the pitch period).
*           fNormPhase  - the starting phase of the output signal 
*                         (calculated by starting time over the pitch period).
*
* Output:   pfOut       - the output data
*
* Globals:  none
*
* Return:   fNormPhase  - the ending phase of the resampled signal. If use 
*                         it as input for next PolyCycleIntp() call, the phases
*                         can be guaranteed to be continous.
*
*******************************************************************************
*
* Implementation/Detailed Description:
*
* References:  
*
*******************************************************************************
*
* Modifications:
*
* Comments:  
*     (1) In order to use fix point technique to speech up the function, the 
*       cycle length has to be in the order of 2 (2^iLog2CycleLength).
*     (2) To use LONG operation (32 bits), iOutLength, iCycleLength and fIndex
*       can not exceed 2^15.
*     (3) In order to speech up FIR operation, the real length of pfCycle has 
*       to be (2^iLog2CycleLength + PP_NUM_SAMPLES). The input pfCycle has 
*       already been shifted by PP_DELAY so that the address of 
*       pfCycle[-PP_DELAY] is valid.
*
* Concerns/TBD:
******************************************************************************/
float PolyCycleIntp( float       *pfOut,
                     int         iOutLength,
                     float       *pfCycle,
                     int         iLog2CycleLength,  
                     float       fResampRate,
                     float       fNormPhase
                   );

/* PreProc.c */
/******************************************************************************
*
* Function:  VoxPreProcess()
*
* Action:    First-order high-pass filter to remove DC.
*
* Input:     piIn -- the input speech signal (16-bit PCM)
*            pfOut -- the point for the output speech
*            N -- number of samples for both input and output signal
*            hPreProc_mblk  -- internal memory for the processing
*
* Output:    pfOut -- the output speech after the preprocessing.
*
* Globals:   none
*
* Return:    void
*******************************************************************************/
void VoxPreProcess(const short *psIn, float *pfOut, int N, void *hPreProcMblk);

/******************************************************************************
*
* Function:  VoxInitPreProc()
*
* Action:    initialize the preprocessing structure.
*
* Input:     hPreProcMblk -- pointer to the preprocess structure (NULL pointer).
*            PP_Coef -- filter coefficient
*
* Output:    hPreProcMblk -- memory allocated preprocess structure.
*
* Globals:   none
*
* Return:    0 : succeed
*            1 : fail
*******************************************************************************/
unsigned short VoxInitPreProc(void **hPreProcMblk, const float PP_Coef);

/******************************************************************************
*
* Function:  VoxFreePreProc()
*
* Action:    free the preprocessing structure.
*
* Input:     hPreProcMblk -- the pointer of the preprocess structure 
*
* Output:    hPreProcMblk -- NULL preprocess structure
*
* Globals:   none
*
* Return:    none
*******************************************************************************/
void VoxFreePreProc(void **hPreProcMblk);

#ifdef __cplusplus
}
#endif

#endif /* _VLIB_FILT */



