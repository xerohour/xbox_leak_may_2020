/******************************************************************************
*                        Voxware Proprietary Material                         *
*                        Copyright 1996, Voxware, Inc                         *
*                            All Rights Reserved                              *
*                                                                             *
*                       DISTRIBUTION PROHIBITED without                       *
*                      written authorization from Voxware                     *
******************************************************************************/

/******************************************************************************
* Filename:       SWSynth.h
*                                                                              
* Purpose:        Sine-wave synthesis for one frame of data.
*                                                                              
* Functions:      VoxSWSynth()
*
* Author/Date:    Original developed by Bob McAulay and Bob Dunn 1/97
*******************************************************************************
* Modifications:                                                               
*                                                                              
* Comments:                                                                    
* 
* Concerns:
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/decode/SWSynth.h_v   1.2   13 Apr 1998 16:17:06   weiwang  $
******************************************************************************/

/*******************************************************************************
*
* Function:  VoxSWSynth()
*
* Action:    Synthesize 10 ms of speech from parameters.
*
* Input:  void   *hHFRegenMblk       -> memory block for HFRegen
*         int     iSCRateSyn         -> current synthesis rate
*         int     iSynSubFrameLength -> number of samples to synthesize
*         float  *pfVEVphase         -> measured phases
*         float  *pfWaveform_1       -> previous synthesized waveform
*         float   fSynPitch          -> pitch for synthesis
*         float   fPv                -> voicing probability for synthesis
*         float  *pfEnv              -> magnitude envelope
*         float  *pfEnv_1            -> previous magnitude envelope
*         float  *pfPhase            -> phase envelope
*         float  *pfPhase_1          -> previous phase envelope
*         float   fInterpFactor      -> envelope interpolation factor
*         int     iUpdatePhaseOffsetFlag -> flag for HFRegen
*         long    *plUVPhaseRandSeed -> address for random seed for unvoiced phases
*
* Output: float  *pfSynSpeech        -> output speech
*
* Globals:   none
*
* Return:    void
********************************************************************************
*
* Modifications:
*
* Comments:      
*
* Concerns/TBD:
*******************************************************************************/
#ifndef SWSYNTH_H
#define SWSYNTH_H

void VoxSWSynth(STACK_R 
                void   *hHFRegenMblk,
                int     iSCRateSyn,
                float  *pfSynSpeech,
                float  *pfVEVphase,
                float  *pfWaveform_1,
                float   fSynPitch,
                float   fPv,
                float  *pfEnv,
                float  *pfEnv_1,
                float  *pfPhase,
                float  *pfPhase_1,
                float   fInterpFactor, 
                int     iUpdatePhaseOffsetFlag,
                long   *plUVPhaseRandSeed
               );

#endif /* SWSYNTH_H */


