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
* Filename:     HFRegen.h
*
* Purpose:   Subroutine of "DecSTC.c" to compute the parameters of the
*               excitation phase, the onset phase and the phase offset (a
*               poor man's glottal pulse) from the quantized STFT phases
*               received at the synthesizer. The excitation phases are
*               computed by subtracting the quantized minimum-phase
*               system phase from the measured phase. The onset-phase,
*               the phase of the fundamental, 
*               is estimated using nonlinear processing. The
*               phase offset is then computed by fitting the straight-line
*               phase to the measured excitation phases. 
*
* Functions:  VoxHFRegeneration(), VoxInitHFRegen(), VoxFreeHFRegen()
*
* Author/Date:  Bob McAulay 21-Mar-97
*
*******************************************************************************
*
* Modifications:
*
* Comments:
*
* Concerns:
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/decode/HFRegen.h_v   1.2   16 Mar 1998 10:55:36   weiwang  $
*
******************************************************************************/
#ifndef HFREGEN_H
#define HFREGEN_H

/*******************************************************************************
*
* Function:  VoxHFRegeneration()
*
* Action:    Generate excitation phase parameters for synthetic phase and mixed
*              synthetic/measured phase systems.
*
* Input:     void  *hHFRegenMblk           -> frame-to frame memory block
*            float *pfVEVphase             -> measures phases in
*            float  fPitchDFT              -> pitch in DFT samples
*            float *pfLogAmp               -> harmonic log magnitudes
*            float *pfMinPhase             -> harmonic minimum-phase phases
*            int    iHarmonics             -> number of harmonics
*            int    iSCRate                -> coding rate flag (3.2k/6.4k)
*            int    iSynSamples            -> number of samples to synthesize
*            int    iUpdatePhaseOffsetFlag -> flag for updating phase offset
*
* Output:    void  *hHFRegenMblk           -> Updated structure
*            float *pfVEVphase             -> measured phase residuals out
*            float *pfOnsetPhase           -> onset phase
*            float *pfBeta                 -> beta value
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

void VoxHFRegeneration( STACK_R 
                        void  *hHFRegenMblk,
                        float *pfVEVphase,
                        float  fPitchDFT,
                        const float *pfLogAmp,
                        const float *pfMinPhase,
                        int    iHarmonics,
                        float *pfOnsetPhase,
                        float *pfBeta,
                        int    iSCRate,
                        int    iSynSamples,
                        int    iUpdatePhaseOffsetFlag
                      );

/*******************************************************************************
*
* Function:  VoxInitHFRegen()
*
* Action:    Initialize memory block for VoxHFRegeneration()
*
* Input:     void **hHFRegenMblk  -> pointer to memory block (invalid)
*
* Output:    void **hHFRegenMblk  -> pointer to memory block (hopefully valid)
*
* Globals:   none
*
* Return:    unsigned short (0/1 success/fail)
********************************************************************************
*
* Modifications:
*
* Comments:  
*
* Concerns/TBD:
*******************************************************************************/

unsigned short VoxInitHFRegen(void **hHFRegenMblk);

/*******************************************************************************
*
* Function:  VoxFreeHFRegen()
*
* Action:    Free memory block for VoxHFRegeneration()
*
* Input:     void **hHFRegenMblk  -> pointer to memory block (hopefully valid)
*
* Output:    none
*
* Globals:   none
*
* Return:    none
********************************************************************************
*
* Modifications:
*
* Comments:  
*
* Concerns/TBD:
*******************************************************************************/

void VoxFreeHFRegen(void **hHFRegenMblk);

#endif /* HFREGEN_H */


