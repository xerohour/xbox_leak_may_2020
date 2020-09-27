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
* Filename:    AsToCs.c
*
* Purpose:    Convert the predictor coefficients to cepstral coefficients using
*               Atal's recursion.
*
* Functions: void asToCs (float gain, float *as, int order_allpole, float *cs,
*                         int order_cepstral)
*
* Author/Date: Bob McAulay
*
*******************************************************************************
*
* Modifications:  Wei Wang, clean up the code 11/97.
*
* Comments: 
*
* Concerns: the prediction coefficients using the format [1 a1 a2 ...]. 
*
* $Header:   /export/phobos/pvcs/pvcsproj/voxlib.prj/AsToCs.c_v   1.6   24 Feb 1998 17:08:56   bobd  $
******************************************************************************/

#include <assert.h>

#include "vLibTran.h"
#include "vLibMath.h" /* VoxMIN() */

#include "xvocver.h"

/* constants */
static const float CsTable[] = 
{
#include "csTab.h"
};

#define MAX_CS_TABLE_ORDER    (sizeof(CsTable)/sizeof(float)-1)

/******************************************************************************
*
* Function:  asToCs()
*
* Action:    Convert the prediction coefficients to cepstrum coefficients
*
* Input:     fGain       -- residual energy in the log domain
*            pfAs        -- prediction coefficients in the format [1 a1 a2 ...]
*            iOrder_As   -- the order of prediction coefficients 
*                             (or the length of pfAs plus 1).
*            pfCs        -- the pointer to the cepstrum coefficients.
*            iLength_Cs  -- the length of pfCs (or the order of cepstrum 
*                             coefficients plus 1).
*
* Output:    pfCs     -- the output cepstrum coefficients
*
* Globals:   none
*
* Return:    void
*******************************************************************************
*
* Implementation/Detailed Description:
*
* References:  Page 115 of "Fundamentals of Speech Recognition" by L. Rabiner 
*                and B. H. Juang, Prentice Hall 1993
*******************************************************************************
*
* Modifications:
*
* Comments:     The format for pfAs[] is:
*                  A(z) = 1 + SUM(1...p) a_p z^(-p) where pfAs[0]=1.
*
* Concerns/TBD: The conventional cepstral coefficients are based on the 
*                 log_base_e allpole envelope. In this function, they are 
*                 based on the half of log_base_2 allpole envelope. So the 
*                 coefficients are scaled by 0.5/ln(2) = 0.72134752. 
******************************************************************************/

void VoxAsToCs( const float  fGain, 
                const float *pfAs, 
                const int    iOrder_As, 
                float       *pfCs, 
                const int    iLength_Cs
              )
 {
   int   m, k;
   int   iOrder_Cs;
   int   P;
   float fSum;

   iOrder_Cs = iLength_Cs - 1;

   assert(iOrder_Cs >= 0);    
   assert(iOrder_Cs <= MAX_CS_TABLE_ORDER);    

   /*-----------------------------------------------------------------------
     chose the order of recursion is min(iOrder_As, iLength_Cs-1)
   -----------------------------------------------------------------------*/
   P = VoxMIN( iOrder_As, iOrder_Cs );

   /*-----------------------------------------------------------------------
     Use the recursion to convert LPC parameter to Cepstrum Coefficients.
   -----------------------------------------------------------------------*/
   pfCs[0] = fGain;      /* gain = .5*log_b2(Residue_Eng) */

   /*----------------------------------------------------------------------
     For cepstrum order smaller than or equal to LPC order.
     using recursion (3.83b) as in P.115 of the reference.
     Note: fSum is the normalized cepstrum here: fSum = m*pfCs[m].
   ----------------------------------------------------------------------*/
   for (m = 1; m <= P; m ++)
   {
      fSum = -(float)m*pfAs[m];
      for (k=1; k<m; k++)
         fSum -= pfAs[k]*pfCs[m-k];
      pfCs[m] = fSum;
   }

   /*----------------------------------------------------------------------
     For cepstrum order larger than LPC order.
     using recursion (3.83c) as in P.115 of the reference.
     Note: fSum is the normalized cepstrum here: fSum = m*pfCs[m].
   ----------------------------------------------------------------------*/
   for (; m <= iOrder_Cs; m++)
   {
      fSum = 0.0F;
      for (k=1; k<=iOrder_As; k++)
         fSum -= pfAs[k]*pfCs[m-k];
      pfCs[m] = fSum;
   }

   /*-----------------------------------------------------------------------
      (1) Remove the normalization (i.e. pfCs[m] = pfCs[m]/m).
      (2) Convert the log_base_e transform to 0.5*log_base_2 transform.
      The constants .5*1.442695041/n are stored in  CsTable[] for speed.
   -----------------------------------------------------------------------*/
   for(m=1; m<=iOrder_Cs; m++)
     pfCs[m] = CsTable[m] * pfCs[m];

}

