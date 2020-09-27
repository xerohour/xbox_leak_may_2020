/*****************************************************************************
*                        Voxware Proprietary Material                       *
*                        Copyright 1996, Voxware Inc.                       *
*                        All Rights Reserved                                *
*                                                                           *
*                        DISTRIBUTION PROHIBITED without                    *
*                        written authorization from Voxware.                *
*****************************************************************************/
/*****************************************************************************
* 
* Filename: VoxDurb.c
*
* Purpose: calculate LPC coefficients using Durbin-Levinson algorithm.
*
* Reference: Optimal signal processing by S. J. Orfanidis
*
* Author:    Wei Wang
*
* Date:      Oct. 07, 1996.
* 
* Modification:  Xiaoqin Sun   Nov. 21, 1997
*
*$Header:   /export/phobos/pvcs/pvcsproj/voxlib.prj/VoxDurb.c_v   1.7   03 Mar 1998 14:38:08   bobd  $
*******************************************************************************/
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "vLibDef.h"
#include "vLibSpch.h"

#include "xvocver.h"

static const float fOverflowThreshold = 0.9999F;

/*****************************************************************************
 * Function: VoxCalcDurbin()
 *
 * Input:  pAutoCorr:     auto-correlation buffer.
 *         iLpcOrder:      LPC order.
 *
 * Output: pLPCoef:       LPC coefficients.
 *         pfPARCOR:      Partial Correlation (reflection) coefficients.
 *         pResidueEng:   residue energy.
 * Return:  None.
 * We define A(z) as: 
 *
 *               A(z) = 1 + SUM(k=1...p) a_k * z^(-k)
 * output is, 1, a[1], a[2]....
 ****************************************************************************/
void VoxDurbin( const float *pAutoCorr, 
                int         iLpcOrder, 
                float       *pfLPCoef,
                float       *pfPARCOR, 
                float       *pfResidEnrg
              )
{
   int             i, j, p;
   float           *pfA;
   float           *pfA_1;
   float          *pfSwap;
   float           fTmp[MAX_LPC_ORDER+1];
   float           fDelta;
   float           fGamma;
   float           fResidEnrg;
   
   assert(iLpcOrder <= MAX_LPC_ORDER);
   assert(pAutoCorr != NULL && pfLPCoef != NULL);
   assert(pfPARCOR != NULL && pfResidEnrg != NULL);

   /*--------------------------------------------------
    check the frame energy first. If frame is almost
    silence, put a small number for the energy.
   --------------------------------------------------*/
   fResidEnrg = pAutoCorr[0];
   if (fResidEnrg == 0.0F)
      fResidEnrg = VERY_SMALL_FLOAT;
   
   /*--------------------------------------------------
    do Leveinson-Durbin iteration.
   --------------------------------------------------*/
   if(iLpcOrder&1)           /* an odd number of iterations */
   {
      pfA    = fTmp;
      pfA_1  = pfLPCoef;
   }
   else                    /* an even number of iterations */
   {  
      pfA   = pfLPCoef;
      pfA_1 = fTmp;
   }

   pfA[0] = pfA_1[0] = 1.0F;
   for (p=0; p<iLpcOrder; p++) 
   {
      fDelta = 0.0F;
      for( i=0; i<=p; i++) 
         fDelta += pfA[i] * pAutoCorr[p+1-i];
   
      /*--------------------------------------------------
         calculate reflection coefficients (Equation 5.3.11
         page 205). Then convergence checking is applied.
      --------------------------------------------------*/
      fGamma =  fDelta / fResidEnrg;

      if ( fGamma > fOverflowThreshold ) 
         fGamma = fOverflowThreshold;
      if ( fGamma < (-fOverflowThreshold) ) 
         fGamma = (-fOverflowThreshold);      

      *pfPARCOR++ = fGamma;
     
      /*------------------------------------------------------
        Move pfA[] to pfA_1[] by swapping pointers
      ------------------------------------------------------*/
      pfSwap = pfA;   /* swap pfA with pfA_1 */
      pfA    = pfA_1;
      pfA_1  = pfSwap;

      /*------------------------------------------------------
        Compute (i+1)th order predictor coefficients
      ------------------------------------------------------*/
      pfA[p+1] = -fGamma;
      for (j=1, i=p; j<=p; j++, i--)
         pfA[j] = pfA_1[j] - fGamma*pfA_1[i];
   
      /*--------------------------------------------------
        iteratively calculate residue energy.
      --------------------------------------------------*/
      fResidEnrg *= 1.0F-fGamma*fGamma;
   }
   *pfResidEnrg = fResidEnrg;
}


