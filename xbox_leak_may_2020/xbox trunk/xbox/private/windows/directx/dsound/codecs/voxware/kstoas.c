/******************************************************************************
*                        Voxware Proprietary Material
*                        Copyright 1997, Voxware, Inc
*                            All Rights Reserved
*
*                       DISTRIBUTION PROHIBITED without
*                      written authorization from Voxware
******************************************************************************/
 
/******************************************************************************
*
* Filename:    KsToAs.c
*
* Purpose:     Convert reflection coefficients to predictor coefficients.
*
* Functions:
*
* Author/Date: Bob Dunn 1/29/97
*
*******************************************************************************
*
* Modifications: clean up by Wei Wang 11/97
*
* Comments:    Algorithm from p 443 of "Digital Processing of Speech Signals" 
*                L.R.Rabiner and R.W.Schafer. Prentice-Hall 1978.
*
* Concerns:   The prediction coefficients are in the format [1 a1 a2 ... ap]
*
* $Header:   /export/phobos/pvcs/pvcsproj/voxlib.prj/KsToAs.c_v   1.4   24 Feb 1998 17:34:54   weiwang  $
******************************************************************************/
#include <assert.h>
#include "vLibDef.h"
#include "vLibTran.h"

#include "xvocver.h"

/******************************************************************************
*
* Function:  VoxKsToAs()
*
* Action:    Convert the reflection coefficients to prediction 
*              coefficients (LPC).
*
* Input:     pfKS     -- the input reflection coefficients.
*            pfAS     -- the pointer for the output LPC coefficients.
*            iOrder   -- the order of both reflection coefficients and LPCs.
*
* Output:    pfAS     -- the output prediction coefficients.
*
* Globals:   none
*
* Return:    void
*******************************************************************************
*
* Implementation/Detailed Description:
*
* References:  p 443 of "Digital Processing of Speech Signals" 
*                L.R.Rabiner and R.W.Schafer. Prentice-Hall 1978.
*******************************************************************************
*
* Modifications:
*
* Comments:     The format for pfAs[] is:
*                  A(z) = 1 + SUM(1...p) a_p z^(-p) where pfAs[0]=1.
*
* Concerns/TBD: (1) Need scratch memory at least MAX_LPC_ORDER+1 (19).
******************************************************************************/

void VoxKsToAs( const float *pfKS, 
                float       *pfAS, 
                int          iOrder
              )
{
   float *pfA_1;              /* point to AS for previous iteration           */
   float *pfSwap;             /* point for swapping the memory */
   float fTemp1[MAX_LPC_ORDER+1]; /*%#  scratch memory #%*/
   float fK;               /* reflection coefficient for current iteration */
   int i, j, k;

   assert(iOrder <= MAX_LPC_ORDER);

   pfAS[0] = 1.0F;

   /*-------------------------------------------------------
     Set up pointers so that the predictor coefficients 
     will be placed in their final location.
   -------------------------------------------------------*/
   if (iOrder&1)           /* an odd number of iterations */
   {
      pfA_1 = pfAS;
      pfAS = fTemp1;      
   }
   else                    /* an even number of iterations */
   {
      pfA_1 = fTemp1;
   }
  
   /*---------------------
      Do the interation.
   ---------------------*/
   for (i=0;i<iOrder; i++)
   {
      pfSwap = pfAS;   /* swap pfA with pfA_1 */
      pfAS = pfA_1;
      pfA_1 = pfSwap;

      fK =  pfKS[i];  /* get (i+1)th reflection coefficient */
      /*------------------------------------------------------
         Compute (i+1)th order predictor coefficients
      ------------------------------------------------------*/
      pfAS[i+1] = -fK;
      for (j=1, k=i; j<=i; j++, k--)
         pfAS[j] = pfA_1[j] - fK*pfA_1[k];
   }
}

