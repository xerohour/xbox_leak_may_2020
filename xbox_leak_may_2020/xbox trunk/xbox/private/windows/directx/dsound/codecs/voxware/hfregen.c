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
* Filename:     HFRegen.c
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
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/decode/HFRegen.c_v   1.7   12 Apr 1998 10:36:54   bobd  $
*
******************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include "vLibMath.h"

#include "VoxMem.h"
#include "codec.h"
#include "rate.h"
#include "trigSC.h"

#include "HFRegen.h"

#include "xvocver.h"

/*-----------------------------------------------------------------
  Sine and cosine tables
-----------------------------------------------------------------*/
extern const float *sinTBL_32;
extern const float *cosTBL_32;
extern const float fRadiansToIndex;

/*-----------------------------------------------------------------
  Amplitude compression for phase coding
-----------------------------------------------------------------*/
#define GAMMA_HFREGENERATION 0.250F  /** was 0.250 **/
 
/*-----------------------------------------------------------------
  Limit bandwidth for onset phase estimation due to 
    minimum phase=0 at PI
-----------------------------------------------------------------*/
#define BANDWIDTH_HFREGENERATION 3000.0F  /** was 1250.0F **/
#define BANDLIMIT                (BANDWIDTH_HFREGENERATION*HZ2DFT)
 
/*-----------------------------------------------------------------
  private structure
-----------------------------------------------------------------*/
typedef struct {
   float fOnsetPhase_1;
   float fBeta_1;
   float fBetaOffset;
   float fPhaseOffset;
   float fPitchDFT_1;
} HFRegenStruct;

/*-----------------------------------------------------------------
  local functions
-----------------------------------------------------------------*/ 
static float fComputeOnsetPhase( STACK_R 
                                 int    iBasePeaks, 
                                 float *pfRE, 
                                 float *pfIM
                               );

static float fComputeBeta( int    iBasePeaks, 
                           float *pfExcitationPhase, 
                           float *pfAmp, 
                           float  fOnsetPhase
                          );

static void SC6Excitation( STACK_R
                           HFRegenStruct *pHFRegenMblk,
                           int            iHarmonics,
                           float          fPitchDFT,
                           int            iSynSamples,
                           int            iUpdatePhaseOffsetFlag,
                           const float    *pfLogAmp,
                           const float    *pfMinPhase,
                           float         *pfVEVphase,
                           float         *pfOnsetPhase,
                           float         *pfBeta
                         );

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
                      )
{
   HFRegenStruct *pHFRegenMblk = (HFRegenStruct*) hHFRegenMblk;

   int   iMhat;

   float fOnsetPhase;
   float fBeta;
   float fDeltaPhase;
   float ftmp;

   assert(hHFRegenMblk != NULL && pfLogAmp != NULL && pfMinPhase != NULL);
   assert(pfOnsetPhase != NULL && pfBeta != NULL);
   
   /*------------------------------------------------------------------------
     Compute the onset phase for 3kb/s using quadratic prediction
   ------------------------------------------------------------------------*/
   if(iSCRate == SC_RATE_3K)
   {
      /*---------------------------------------------------------------------
        Apply quadratic phase projection
      ---------------------------------------------------------------------*/
      fDeltaPhase =  0.5F*TWOPIONFFT*(fPitchDFT+pHFRegenMblk->fPitchDFT_1)
                     *iSynSamples;
      fOnsetPhase = pHFRegenMblk->fOnsetPhase_1 + fDeltaPhase;

      /*---------------------------------------------------------------------
        Use previous Beta value for consistency
      ---------------------------------------------------------------------*/
      fBeta = pHFRegenMblk->fBeta_1;

   } else {

      SC6Excitation( STACK_S pHFRegenMblk, iHarmonics, fPitchDFT, iSynSamples, 
                     iUpdatePhaseOffsetFlag, pfLogAmp, pfMinPhase, pfVEVphase, 
                     &fOnsetPhase, &fBeta);

   }

   /*-----------------------------------------------------------------------
     Compute modulo 2PI of the onset phase
   -----------------------------------------------------------------------*/
   ftmp=fOnsetPhase*INVTWOPI;
   VoxROUND2(ftmp,iMhat);
   fOnsetPhase -= iMhat*TWOPI;

   /*-----------------------------------------------------------------------
     Set output parameter values
   -----------------------------------------------------------------------*/
   *pfOnsetPhase = fOnsetPhase;
   *pfBeta       = fBeta;

   /*-----------------------------------------------------------------------
     Save for next time
   -----------------------------------------------------------------------*/
   pHFRegenMblk->fOnsetPhase_1 = fOnsetPhase;
   pHFRegenMblk->fPitchDFT_1   = fPitchDFT;

} /*VoxHFRegen*/

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

unsigned short VoxInitHFRegen(void **hHFRegenMblk)
{
   HFRegenStruct  *pHFRegenMblk;
   unsigned short  usStatus = 0;

   if(VOX_MEM_INIT(pHFRegenMblk=*hHFRegenMblk,1, sizeof(HFRegenStruct)))
   {
      usStatus = 1;
   } else {

    /*---------------------------------------------------------------
      VOX_MEM_INIT sets pHFRegenMblk to zero on the Sun and PC, so
      to reduce code size we can skip the next four lines of code.
      On some platforms this code will be necessary.

      pHFRegenMblk->fOnsetPhase_1 = 0.0F;
      pHFRegenMblk->fBeta_1       = 0.0F;
      pHFRegenMblk->fBetaOffset   = 0.0F;
      pHFRegenMblk->fPhaseOffset  = 0.0F;
    ---------------------------------------------------------------*/
      pHFRegenMblk->fPitchDFT_1   = (float)INITIAL_PITCH_DFT; 
   }

   return usStatus;
} /* VoxInitHFRegen */

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
* Return:    unsigned short (0 for always successful)
********************************************************************************
*
* Modifications:
*
* Comments:  
*
* Concerns/TBD:
*******************************************************************************/

void VoxFreeHFRegen(void **hHFRegenMblk)
{
   if (*hHFRegenMblk)
      VOX_MEM_FREE(*hHFRegenMblk);
} /* VoxFreeHFRegen */

/*******************************************************************************
*
* Function:  fComputeOnsetPhase()
*
* Action:    Compute the onset phase from real and imaginary parts of 
*              baseband peaks.
*
* Input:     int    iBasePeaks  -> number of baseband peaks
*            float *pfRE        -> real part of baseband peaks
*            float *pfIM        -> imaginary part of baseband peaks
*
* Output:    none
*
* Globals:   none
*
* Return:    float fOnsetPhase  -> the onset phase
********************************************************************************
*
* Modifications:
*
* Comments:  
*
* Concerns/TBD:
*******************************************************************************/

static float fComputeOnsetPhase(STACK_R int    iBasePeaks, 
                                        float *pfRE, 
                                        float *pfIM)
{
   int   j, k;
   int   iIndex;
   int   iMhat;
   float fOnsetPhase;
   float fNewOnsetPhase;
   float fTemp1;
   float fTemp2;
   float ftmp;

STACK_INIT
   float fXR[SC6_PHASES];
   float fYR[SC6_PHASES];
STACK_INIT_END

   STACK_ATTACH(float *, fXR)
   STACK_ATTACH(float *, fYR)

   STACK_START

   /*-----------------------------------------------------------------------
     Compute the onset time for the first phase difference 
   -----------------------------------------------------------------------*/
   memset(fXR, 0, SC6_PHASES*sizeof(float));
   memset(fYR, 0, SC6_PHASES*sizeof(float));

   for (j = 0; j < iBasePeaks; j++)
   {  
     for (k=j+1, iIndex=1; k < iBasePeaks; k++, iIndex++)
      {
         fXR[iIndex] += (pfRE[j]*pfRE[k] + pfIM[j]*pfIM[k]);
         fYR[iIndex] += (pfRE[j]*pfIM[k] - pfIM[j]*pfRE[k]); 
      }
   }
   fOnsetPhase = VoxATAN2(fYR[1], fXR[1]);


   /*-----------------------------------------------------------------------
       Compute the onset time for the 2nd-order phase difference 
   -----------------------------------------------------------------------*/
   fTemp2 = 2.0F*fOnsetPhase;
   fTemp1 = VoxATAN2(fYR[2], fXR[2]);
   ftmp = (fTemp2 - fTemp1)*INVTWOPI;
   VoxROUND2(ftmp, iMhat);
   fNewOnsetPhase = 0.5F*(fTemp1 + (float)iMhat*TWOPI);
   fOnsetPhase = 0.5F*(fOnsetPhase + fNewOnsetPhase);

   STACK_END

   return fOnsetPhase;
} /* fComputeOnsetPhase */

/*******************************************************************************
*
* Function:  fComputeBeta()
*
* Action:    Compute Beta (the phase offset at DC)
*
* Input:     int    iBasePeaks        -> number of basaeband peaks
*            float *pfExcitationPhase -> excitation phases
*            float *pfAmp             -> amplitudes (compressed, but not log 2)
*            float  fOnsetPhase       -> estimated onset phase
*
* Output:    none
*
* Globals:   none
*
* Return:    float fBeta (the phase offset at DC)
********************************************************************************
*
* Modifications:
*
* Comments:  
*
* Concerns/TBD:
*******************************************************************************/

static float fComputeBeta( int    iBasePeaks, 
                           float *pfExcitationPhase, 
                           float *pfAmp, 
                           float  fOnsetPhase)
{
   int   j;
   int   iResPhase;
   float fRESum = 0.0F;
   float fIMSum = 0.0F;
   float fModelPhase = 0.0F;
   float fResPhase;
   float fBeta;

   for (j = 0; j < iBasePeaks; j++)
   {
      /*-------------------------------------------------
        Compute residual phase
      -------------------------------------------------*/
      fModelPhase += fOnsetPhase;
      fResPhase = pfExcitationPhase[j] - fModelPhase;

      /*-------------------------------------------------
        Compute trig table index.

        NOTE: This assumes a 2's complement integer
              representation.  Using the trig mask 
              (iResPhase&TRIG_MASK) yeilds the correct
              positive index, even if iResPhase is
              negative.  This would not be true for
              signed magnitude integer representation.
      -------------------------------------------------*/
      fResPhase *= fRadiansToIndex;
      VoxROUND2( fResPhase, iResPhase );
      iResPhase &= TRIG_MASK;

      /*-------------------------------------------------
        Accumulate real and imaginary parts using
          trig tables
      -------------------------------------------------*/
      fRESum += pfAmp[j]*cosTBL_32[iResPhase];
      fIMSum += pfAmp[j]*sinTBL_32[iResPhase];
   }

   fBeta = VoxATAN2(fIMSum, fRESum);

   return fBeta;
} /* fComputeBeta */

/*******************************************************************************
*
* Function:  SC6Excitation()
*
* Action:    Compute onset phase, beta factor and measured phase residuals
*              for generation of excitation phase in SC6.
*
* Input:     HFRegenStruct *pHFRegenMblk  -> frame-to frame memory block
*            int            iHarmonics    -> number of harmonics
*            float          fPitchDFT     -> pitch in DFT samples
*            int            iSynSamples   -> number of samples to synthesize
*            float         *pfLogAmp      -> harmonics log amplitudes
*            float         *pfMinPhase    -> harmonic minimum-phase phases
*            float         *pfVEVphase    -> measured phases (in)
*            int            iUpdatePhaseOffsetFlag -> update phase offset flag
*
* Output:    float         *pfOnsetPhase  -> the onset phase
*            float         *pfBeta        -> Beta (DC phase offset)
*            float         *pfVEVphase    -> measured phase residuals (out)
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

static void SC6Excitation( STACK_R 
                           HFRegenStruct *pHFRegenMblk, 
                           int            iHarmonics, 
                           float          fPitchDFT, 
                           int            iSynSamples, 
                           int            iUpdatePhaseOffsetFlag,
                           const float   *pfLogAmp,
                           const float   *pfMinPhase, 
                           float         *pfVEVphase, 
                           float         *pfOnsetPhase,
                           float         *pfBeta
                         )
{
   int i;
   int iMpeaks;
   int iBasePeaks;
   int iPhase;

   float fFreq;
   float fPhase;
   float fOnsetPhase;
   float fModelPhase;
   float fDeltaPhase;
   float fBeta;
STACK_INIT
   float pfAmp[SC6_PHASES];
   float pfRE[SC6_PHASES];
   float pfIM[SC6_PHASES];
STACK_INIT_END

   STACK_ATTACH(float *, pfAmp)
   STACK_ATTACH(float *, pfRE)
   STACK_ATTACH(float *, pfIM)

STACK_START

   /*-----------------------------------------------------------------------
     Set actual number of measured phases
   -----------------------------------------------------------------------*/
   iMpeaks = VoxMIN( SC6_PHASES, iHarmonics );

   /*-----------------------------------------------------------------------
     Limit bandwidth of onset phase estimator so that it will not be
       affected by the fact that the system phase is zero at PI
   -----------------------------------------------------------------------*/
   for (iBasePeaks=0, fFreq=fPitchDFT; iBasePeaks<iMpeaks; 
        iBasePeaks++, fFreq+=fPitchDFT)
     {
        if (fFreq > BANDLIMIT)
           break;
     }

   /*-----------------------------------------------------------------------
     Compute the real and imaginary parts of the excitation phase
   -----------------------------------------------------------------------*/
   for (i = 0; i < iMpeaks; i++)
   {
      /*-------------------------------------------------
        Remove minimum-phase system phase
      -------------------------------------------------*/
      pfVEVphase[i] -= pfMinPhase[i];

      /*-------------------------------------------------
        Compute trig table index.
 
        NOTE: This assumes a 2's complement integer
              representation.  Using the trig mask 
              (iResPhase&TRIG_MASK) yeilds the correct
              positive index, even if iResPhase is
              negative.  This would not be true for  
              signed magnitude integer representation.
      -------------------------------------------------*/
      fPhase = pfVEVphase[i]*fRadiansToIndex;
      VoxROUND2( fPhase, iPhase );
      iPhase &= TRIG_MASK;

      /*-------------------------------------------------
        Compute real and imaginary parts of compressed 
          amplitude
      -------------------------------------------------*/
      pfAmp[i] = fInvLog2(GAMMA_HFREGENERATION*pfLogAmp[i]);
      pfRE[i]  = pfAmp[i]*cosTBL_32[iPhase];
      pfIM[i]  = pfAmp[i]*sinTBL_32[iPhase];
   }

   /*-----------------------------------------------------------------------
     Compute onset phase
   -----------------------------------------------------------------------*/
   fOnsetPhase = fComputeOnsetPhase(STACK_S iBasePeaks, pfRE, pfIM);

   /*-----------------------------------------------------------------------
        Determine the phase offset at DC 
   -----------------------------------------------------------------------*/
   fBeta = fComputeBeta( iBasePeaks, pfVEVphase, pfAmp, fOnsetPhase);

   /*-----------------------------------------------------------------------
        Compute the phase residuals (NOTE: these are WRAPPED)
   -----------------------------------------------------------------------*/
   fModelPhase = fBeta;
   for (i = 0; i < iMpeaks; i++)
   {
      fModelPhase   += fOnsetPhase;
      pfVEVphase[i] -= fModelPhase;
   }

   /*-----------------------------------------------------------------------
     update phase offset and beta when frame loss occurs
   -----------------------------------------------------------------------*/
   if (iUpdatePhaseOffsetFlag)
   {
      fDeltaPhase =  (0.5F*TWOPIONFFT)*(fPitchDFT+pHFRegenMblk->fPitchDFT_1)
                     *iSynSamples;
      pHFRegenMblk->fPhaseOffset = pHFRegenMblk->fOnsetPhase_1 +
                                   fDeltaPhase - fOnsetPhase;
      
      pHFRegenMblk->fBetaOffset = pHFRegenMblk->fBeta_1 - fBeta;
   }

   fBeta       += pHFRegenMblk->fBetaOffset;
   fOnsetPhase += pHFRegenMblk->fPhaseOffset;

   /*-----------------------------------------------------------------------
     Save this parameter for next time
   -----------------------------------------------------------------------*/
   pHFRegenMblk->fBeta_1 = fBeta;

   /*-----------------------------------------------------------------------
     Set output values
   -----------------------------------------------------------------------*/
   *pfOnsetPhase = fOnsetPhase;
   *pfBeta = fBeta;

STACK_END
} /* SC6Excitation */

