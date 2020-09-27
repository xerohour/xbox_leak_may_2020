/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/

/*******************************************************************************
* Filename:       Voicing.c
*                                                                              
* Purpose:        Single-band voicing probability calculation
*                 using the peaks out of the square-law nonlinearity 
*                                                                              
* Functions:      VoxSTCVoicing()
*                 ModifyPv()
*
* Author/Date:    Bob McAulay, 11/10/96                     
********************************************************************************
* Modifications:Ilan Berci/ 10/96 
*                 ModifyPv() added by Wei Wang
*                                                                              
* Comments:                                                                    
* 
* Concerns:
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/pea/Voicing.c_v   1.5   07 Jul 1998 18:10:12   bobd  $
*******************************************************************************/
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <limits.h>
#include <string.h>

#include "vLibMath.h"
#include "vLibVec.h"

#include "VoxMem.h"
#include "codec.h"
#include "Voicing.h"
#include "Compress.h"
#include "Seevoc.h"
#include "RefineC0.h"
#include "SqLawPks.h"

#include "xvocver.h"


#define PV_BANDEDGE     (int)(1350.0*HZ2DFT)   /***** cut off at 1350 Hz ******/
#define MIN_PV_HARM     5                      /***** will be change to 4 *****/
#define MIN_QPV         0.071428572F           /***** 0.5/7               *****/

/*------------------------------------------------------
  The cutoff is 2500 Hz.  Be careful when changing this
    value.  The value of fCutOffBand sets an upper bound 
    on the value of fBaseBand, and in the code below
    we assume that round(fBaseBand)+1 is less than or
    equal to NFFT.
------------------------------------------------------*/
static const float fCutOffBand = (float) ((NFFT*5)>>4);

static float ComputePv( float fdBmin,
                        float fdBmax,
                        float fRho
                      );

/******************************************************************************
*
* Function:
*
* Action: 
*
* Input: 
*
* Output:
*
* Globals:
*
* Return: 
*******************************************************************************
*
* Implementation/Detailed Description:
*
* References:
*******************************************************************************
*
* Modifications:
*
* Comments:
*
* Concerns/TBD:
******************************************************************************/
void VoxSTCVoicing( STACK_R 
                    float *pfPv_km1, 
                    float *pfSeevocEnv, 
                    float *pfPitchPeriod, 
                    float *pfPv, 
                    float *pfPeakPwr, 
                    float *pfPeakFrq, 
                    int    iPeakCount
                  )
{
  int j, k, iIndex;
  int iBaseBand, iBasePeakCount, NuPeaks;

  float fCoarsePitchDFT, ratio;
  float LoBandEdge, fBaseBand, NuEnergy;
  float fRhoRefined, fRefinedPitchPeriod, fCoarsePitchPeriod;
  float fTemp,fdBmin, PvLastTime, PvFinal;
  float fMaxMagSq;

   /*-----------------------------------------------------------------
     Compute the band edges in DFT points 
   -----------------------------------------------------------------*/
   fCoarsePitchPeriod = *pfPitchPeriod;
   fCoarsePitchDFT = (float)NFFT/ fCoarsePitchPeriod;

   iBasePeakCount = VoxFLOOR((PV_BANDEDGE/(float)NFFT)*fCoarsePitchPeriod);
   if(iBasePeakCount < MIN_PV_HARM)
      iBasePeakCount = MIN_PV_HARM;


   fBaseBand = (iBasePeakCount + 0.125F)*fCoarsePitchDFT;  /** was 0.5F **/
   if (fBaseBand > fCutOffBand)                /***** extend to 2500.0Hz *****/
     fBaseBand = fCutOffBand;

  
   VoxROUND2pos(fBaseBand, iBaseBand);

   /*-----------------------------------------------------------------
     Here we add 1 to iBaseBand.  The peaks to be used are
       0 through round(fBaseBand), including 0 and round(fBaseBand).
       Since we include both endpoints, the value of iBaseBand should 
       be round(fBaseBand)+1.
   -----------------------------------------------------------------*/
   iBaseBand++;
   assert( iBaseBand <= NFFT );

   /*-----------------------------------------------------------------
     Compute the number of baseband peaks 
   -----------------------------------------------------------------*/
   for (k = 0; k < iPeakCount; k++)
   {
      if (pfPeakFrq[k] > fBaseBand)
            break;
   }
   iBasePeakCount = k;

   if (iBasePeakCount>0)
   {
      /*-----------------------------------------------------------------
        Find the maximum magnitude of the baseband peaks and 
          the baseband region of pfSeevocEnv[].
      -----------------------------------------------------------------*/
      FindMaxValueFlt( pfPeakPwr, iBasePeakCount, &k, &fMaxMagSq );
      FindMaxValueFlt( pfSeevocEnv, iBaseBand, &k, &fTemp );
      fMaxMagSq = VoxMAX( fMaxMagSq, fTemp );
   
      /*-----------------------------------------------------------------
        Compress the baseband STFT peaks in place.
      -----------------------------------------------------------------*/
      ScaleAndCompress( pfPeakPwr, iBasePeakCount, fMaxMagSq, pfPeakPwr);
   
      {
         /*-----------------------------------------------------------------
           The length of fFltSee[] only need cover the baseband harmonics.
             We should figure out how to make this array smaller.
         -----------------------------------------------------------------*/
         STACK_INIT
           float  fFltSee[NFFT_2];         /** flattend Seevoc envelope      **/
         STACK_INIT_END
           STACK_ATTACH(float *, fFltSee)
         STACK_START
    
   
         /*-----------------------------------------------------------------
           Compress the baseband magnitude-squared seevoc flattop envelope.
             This operation is done out of place since the seevoc envelope
             will be needed later.
         -----------------------------------------------------------------*/
         ScaleAndCompress( pfSeevocEnv, iBaseBand, fMaxMagSq, fFltSee);
   
         /*-----------------------------------------------------------------
           Suppress the peaks below the compressed flattop envelope
         -----------------------------------------------------------------*/
         for (j = 0; j < iBasePeakCount; j++)
         {
            fTemp = pfPeakFrq[j];
            VoxROUND2pos(fTemp, iIndex);
   
            if(pfPeakPwr[j] < fFltSee[iIndex])
            {
               ratio = pfPeakPwr[j]/fFltSee[iIndex];
               fTemp = ratio*ratio;
               pfPeakPwr[j] *= (fTemp*fTemp);
            }
         }
   
         STACK_END
      }
   
      {
         STACK_INIT
           float  fNuAmp[MAXHARM];
           float  fNuFrq[MAXHARM];
         STACK_INIT_END
           STACK_ATTACH(float *, fNuAmp)
           STACK_ATTACH(float *, fNuFrq)
         STACK_START
   
         /*-----------------------------------------------------------------
           Compute the sine-wave representation of the square-law output 
         -----------------------------------------------------------------*/
         LoBandEdge = SQVOC_LO_EDGE*HZ2DFT;  /** ~850Hz **/
   
         VoxSquareLawPeaks( pfPeakPwr, pfPeakFrq, iBasePeakCount, 
                            LoBandEdge, fNuAmp, fNuFrq, &NuPeaks ); 
   
         /*-----------------------------------------------------------------
           Refine the pitch about the previously estimated value
         -----------------------------------------------------------------*/
         VoxRefineC0(STACK_S fCoarsePitchPeriod, fNuAmp, fNuFrq, NuPeaks, 
                     &NuEnergy, &fRefinedPitchPeriod, &fRhoRefined); 
   
         STACK_END
      }
   } else { /* if (iBasePeakCount>0) */
      /*------------------------------------------------------------
        There are no peaks in the baseband!
      ------------------------------------------------------------*/
      fRefinedPitchPeriod = fCoarsePitchPeriod;
      fRhoRefined         = 0.0;
   } /* end of else for condition: (iBasePeakCount>0) */


   /*-----------------------------------------------------------------
     Compute the coarse pitch voicing probability
   -----------------------------------------------------------------*/
   PvLastTime = *pfPv_km1;
   fdBmin     = DBMID + (DBMIN - DBMID)*PvLastTime;
   PvFinal    = ComputePv (fdBmin, DBMAX, fRhoRefined);
   *pfPv_km1  = PvFinal;

   /*-----------------------------------------------------------------
     Use the refined pitch and voicing 
   -----------------------------------------------------------------*/
   *pfPitchPeriod = fRefinedPitchPeriod;
   *pfPv          = PvFinal;

} /*VoxVoicing()*/

/******************************************************************************
*
* Function:   ComputePv()
*
* Action:     Subroutine to compute the voicing probability
*               according to TFQ's linear-log rule with RJM's added pitch
*               correlation weighting which increases the voicing probablity
*               with an increasing number of pitch correlations
*
* Input: 
*
* Output:
*
* Globals:    none
*
* Return:     none
*******************************************************************************
*
* Implementation/Detailed Description:
*
* References:
*******************************************************************************
*
* Modifications:
*
* Comments:
*
* Concerns/TBD:
******************************************************************************/

static float ComputePv( float fdBmin, 
                        float fdBmax, 
                        float fRho
                      ) 
{
   float fSNRdB;
   float fPv;

   /*-----------------------------------------------------------------
     Check for numerical instabilities
   -----------------------------------------------------------------*/
   if (fRho <= 0.0F)
            fRho = 0.000001F;
   if (fRho >= 1.0F)
            fRho = 0.999999F;
 
   /*-----------------------------------------------------------------
     Convert fRho to dB
   -----------------------------------------------------------------*/
   fSNRdB = -10.0F*(float)log10(1.0F - fRho);          /* SNR in dB */

   /*-----------------------------------------------------------------
     Voicing probability is linear in dB SNR
   -----------------------------------------------------------------*/
   if (fSNRdB < fdBmin)
      fPv = 0.0F;      /* fully unvoiced */

   else if (fSNRdB > fdBmax)
      fPv = 1.0F;      /* fully voiced */

   else 
      /*------------------------------------------------
        if (fSNRdB >= fdBmin && fSNRdB <= fdBmax) 
             (partial voicing)
      ------------------------------------------------*/
      fPv = (fSNRdB - fdBmin)/(fdBmax - fdBmin);

   return( fPv );
} /* ComputePv() */


/******************************************************************************
*
* Function:  ModifyPv()
*
* Action:    Compute a modified voicing probability based on correlation
*              coefficients, current and previous pitch values, and 
*              current and previous voicing probabilities.
*
* Input:     float fCorrCost  --> correlation coefficient
*            float fPv        --> current voicing probability
*            float fPv_1      --> previous voicing probability
*            float fPitch     --> current pitch period
*            float fPitch_1   --> previous pitch period
*
* Output:    none
*
* Globals:   none
*
* Return:    float fPv (the modified voicing probability)
*******************************************************************************
*
* Implementation/Detailed Description:
*
* References:
*******************************************************************************
*
* Modifications:
*
* Comments:
*
* Concerns/TBD:
******************************************************************************/
float ModifyPv( float fCorrCost, 
                float fPv, 
                float fPv_1, 
                float fPitch,
                float fPitch_1
              )
{     
  float fEstPv;

  /*-----------------------------------------------------------------
    If fCorrCost is negative, fPv should be 0.
  -----------------------------------------------------------------*/
  if (fCorrCost <= 0.01F)   /* unvoiced speech */
    fPv = 0.0F;

  /*-----------------------------------------------------------------
    If fCorrCost is large enough and fPv is too small, we have to
      reset fPv to a larger number.
  -----------------------------------------------------------------*/
  if (fCorrCost >= 0.45F)   /* voiced speech */
  {
    fEstPv = fCorrCost - 0.45F;

    if ((fEstPv * (fPitch * 0.5F)) < 1.0F)    /* at least one harmonic */
      fEstPv += MIN_QPV;

    if (fEstPv > fPv)
      fPv = fEstPv;
  }

  /*-----------------------------------------------------------------
    Now smooth the fPv
  -----------------------------------------------------------------*/
  if (fPv_1 > fPv && fPv_1 >= MIN_QPV) 
  {
    if ((fPitch < 1.25F * fPitch_1)&&(fPitch > 0.8F * fPitch_1))
      fPv = 0.7F*fPv + 0.3F*fPv_1;
    
    if (fPv < MIN_QPV)
      fPv += MIN_QPV;
  }
  
  return fPv;

} /* ModifyPv() */

