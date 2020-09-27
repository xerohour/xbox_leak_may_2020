/******************************************************************************
*                        Voxware Proprietary Material
*                        Copyright 1996, Voxware, Inc
*                            All Rights Reserved
*
*                       DISTRIBUTION PROHIBITED without
*                      written authorization from Voxware
******************************************************************************/
 
/******************************************************************************
*
* Filename:        SVqPhase.c
*
* Purpose:         Pairwise vector quantization of the 10ms and 20ms phases.
*                  The 10ms and 20ms pitches are use to predict the 20ms phase
*                  from the 10ms phase. The measured 20ms phase and the
*                  predicted 20ms phase then form a pair that are
*                  vector-quantized. 
*
* Functions:     VoxSVQEncPhase, VoxSVQDecPhase, VoxEncPhasePrediction,
*                VoxDecPhasePrediction
*
* Author/Date:     Bob Dunn 4/18/97 and Bob McAulay 4/30/97
*
*******************************************************************************
*
* Modifications:   With Phase prediction to increase correlation
*                  Hard wired for a sub-vector length of 2 for speed
*
* Comments:    
*
* Concerns:
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/quan/SVqPhase.h_v   1.3   13 Apr 1998 16:17:56   weiwang  $
*
******************************************************************************/
#ifndef SVQPHASE_H
#define SVQPHASE_H


/******************************************************************************
*
* Function:  VoxSVQEncPhase
*
* Action:    Use this module to VQ pairwise predictive phase coding.
*
* Input:  pfPhaseErr --  phase errors
*         pfPhase    --  end-frame phases
*         piIndices -- pointer to the output indices
*
* Output: piIndices -- output quantization indices
*
* Globals:   none
*
* Return:    none
*******************************************************************************/
void VoxSVQEncPhase(float *pfPhase_Err, float *pfPhase, int *piIndices);


/******************************************************************************
*
* Function:  VoxSVQDecPhase
*
* Action:    Use this module to decode the phase pair
*
* Input:  piIndices -- quantization indices
*         pfPhase_Err -- pointers to predictive phase errors
*         pfPhase     -- pointer to the end-frame phases
*
* Output: pfPhase_Err -- predictive phase errors
*         pfPhase     -- the end-frame phases
*
* Globals:   none
*
* Return:    none
*******************************************************************************/
void VoxSVQDecPhase(const int *piIndices, float *pfPhase_Err, float *pfPhase);


/******************************************************************************
*
* Function:  VoxEncPhasePrediction
*
* Action:    Predict the 10ms phases and output the predictive error
*
* Input:  fPitch_Mid -- middle-frame pitch
*         fPitch     -- end-frame pitch
*         pfPhaseMid -- middle-frame phases
*         pfPhase    -- end-frame phases
*
* Output: pfPhaseMid -- predictive phase errors
*         pfPhase    -- end-frame phases
*
* Globals:   none
*
* Return:    none
*******************************************************************************/
void VoxEncPhasePrediction (float fPitch_Mid, float fPitch, 
                            float *pfPhaseMid, float *pfPhase);


/******************************************************************************
*
* Function: VoxDecPhasePrediction
*
* Action:  Estimate the 10ms phases from 20ms phases and 10ms predictive errors.
*
* Input:  fPitchMid -- middle-frame pitch
*         fPitch    -- end-frame pitch
*         pfPhaseMid -- predictive phase errors
*         pfPhase -- end-frame phases
*
* Output: pfPhaseMid -- middle-frame phases
*         pfPhase    -- end-frame phases
*
* Globals:   none
*
* Return:    none
*******************************************************************************/
void VoxDecPhasePrediction(float fPitchMid, float fPitch, 
                           float *pfPhaseMid, float *pfPhase);

#endif /* SVQPHASE_H */
