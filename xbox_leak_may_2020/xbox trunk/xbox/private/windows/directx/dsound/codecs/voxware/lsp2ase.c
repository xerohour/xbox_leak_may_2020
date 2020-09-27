/*******************************************************************************
*                        Voxware Proprietary Material
*                        Copyright 1996, Voxware, Inc
*                            All Rights Reserved
*
*                       DISTRIBUTION PROHIBITED without
*                      written authorization from Voxware
*******************************************************************************/
 
/*******************************************************************************
*
* Filename:     lsp2ase.c
*
* Purpose:      Convert LSPs to predictor coefficients for even order all-pole
*                 models.
*
* Functions:   void VoxLspToAsEven()
*
* Author/Date: Bob Dunn 1/28/98
*
********************************************************************************
*
* Modifications:
*
* Comments:
*
* Concerns:
*
* $Header:   G:/pvcsproj/voxlib.prj/lsp2ase.c_v   1.5   03 Mar 1998 09:44:06   weiwang  $
*******************************************************************************/
#include <math.h>
#include <assert.h>
 
#include "vLibDef.h"
#include "vLibTran.h"

#include "xvocver.h"

/*******************************************************************************
* Function:       void VoxLspToAsEven()
*
* Action:         Compute order predictor coefficients from LSPs for
*                   even ordered all-pole models.
*
* Input:          const float *pfLSP       -> LSPs
*                 int         iOrder       -> all-pole model order
*                 float       fScaleFactor -> scale factor to apply to LSPs
*                                               (when multiplied by fScaleFactor
*                                               the LSPs should range from 
*                                               0 to 0.5).
*
* Output:         float       *pfAs        -> 10th order predictor coefficients
*                                                where pfAs[0] = A_0 = 1.0
*
* Globals:        none
*
* Return:         void
********************************************************************************
* Implementation/Detailed Description:
*
*
* References:  "Speech Analysis and Synthesis Methods Developed at ECL in
*                NTT - From LPC to LSP -", Noboru Sugamura and Fumitada 
*                Itakura, Speech Communication 5 (1986) pp 199-215.
*
*              "Line Spectrum Pair (LSP) and Speech Data Compression",
*                Frank k. Soong and Bing-Hwang Juang, Proc. ICASSP 1984,
*                pp 1.10.1-1.10.4
*
*              "Application of Line-Spectrum Pairs to Low-Bit_rate Speech
*                Encoders", George S. Kang and Lawrence J. Fransen, Proc.
*                ICASSP 1985, PP 7.3.1-7.3.4
********************************************************************************
* Modifications:
*
* Comments:    The inverse filter is defined as:
*
*                      A(Z) = 1 + SUM(1...p) A_p Z^(-p)
*
*              The LSPs are normalized in frequency and should range
*                from 0 to 0.5.  Since many codecs require LSPs in
*                Hertz, fScaleFactor is included to allow the conversion
*                from Hertz to normalized frequency.  If the input LSPs are 
*                in Hertz, fScaleFactor should be set to one over the 
*                sampling rate.  If the LSPs are normalized in frequency
*                (ranging from 0 to 0.5) fScaleFactor should be set to 1.0.
*
* Concerns/TBD:
*******************************************************************************/

void VoxLspToAsEven( const float  *pfLSP, 
                     int          iOrder, 
                     float        fScaleFactor,
                     float        *pfAs 
                   )
{
   int   i, j;
   int   iOrder2 = iOrder>>1;

   float fScale;
   float fRp[(MAX_LPC_ORDER>>1)+1];
   float fRq[(MAX_LPC_ORDER>>1)+1];
   float fDp;
   float fDq;

   
   /*------------------------------------------------------
     Make sure the model order is even and that it is less 
       than or equal to MAX_LPC_ORDER.  
   ------------------------------------------------------*/ 
   assert( !(iOrder&1) );
   assert( iOrder <= MAX_LPC_ORDER );
   assert( pfLSP );
   assert( pfAs );

   fScale = fScaleFactor*DB_PI;

   /*------------------------------------------------------
     Compute reduced polynomials Rp(z) and Rq(z).
       These are computed from:

       Rp(z) = PROD(i=1...N/2) (1 + Dp_i z^(-1) + z^(-2))
       Rq(z) = PROD(i=1...N/2) (1 + Dq_i z^(-1) + z^(-2))

       where:

          Dp_i = -2.0*cos( 2*PI * LSP(2*i-1) )
          Dq_i = -2.0*cos( 2*PI * LSP(2*i) )

       Due to the symmetry, only half of the coefficients 
       of Rp(z) and Rq(z) need to be computed.  Also,
       in each term of the product, the coefficients of 
       z^0 and z^(-2) are 1, so multiplication 
       by either of these two coefficients is replaced 
       by a simple assignment operation.
   ------------------------------------------------------*/


   /*------------------------------------------------------
     Assign coefficient values of the first stage of the
       product.  The values of fRp[2] and fRq[2] are not
       assigned since they are symmetric with fRp[0] and 
       fRq[0].
   ------------------------------------------------------*/
   fRp[0] = 1.0F;
   fRq[0] = 1.0F;

   /*------------------------------------------------------
     Compute the coefficients Dp_0 and Dq_0 from the LSPs.
   ------------------------------------------------------*/
   fRp[1] = -2.0F * (float) cos( fScale * (*pfLSP++) );
   fRq[1] = -2.0F * (float) cos( fScale * (*pfLSP++) );

   /*------------------------------------------------------
     Multiply out each sucessive stage of the product.
   ------------------------------------------------------*/
   for (i=1; i<iOrder2; i++)
   {

      /*------------------------------------------------------
        Compute the coefficients Dp_i and Dq_i from the LSPs.
      ------------------------------------------------------*/
      fDp = -2.0F * (float) cos( fScale * (*pfLSP++) );
      fDq = -2.0F * (float) cos( fScale * (*pfLSP++) );

      /*------------------------------------------------------
         This first step in the product is actually:

           fRp[i+1] = fRp[i+1] + fDp*fRp[i] + fRp[i-1]

         but there is no value for fRp[i+1] from the
         previous stage of the product since only half
         of the symmetric coefficients are computed.  
         The value of fRp[i+1] from the previous stage
         can be found from the symmetry:

            fRp[i+1] = fRp[i-1]

         so the new value of fRp[i+1] is computed as:

            fRp[i+1] = fRp[i-1] + fDp*fRp[i] + fRp[i-1]

      ------------------------------------------------------*/
      fRp[i+1] = fDp*fRp[i] + 2.0F*fRp[i-1];
      fRq[i+1] = fDq*fRq[i] + 2.0F*fRq[i-1];

      for (j=i; j>1; j--)
      {
         fRp[j] = fRp[j] + fDp*fRp[j-1] + fRp[j-2];
         fRq[j] = fRq[j] + fDq*fRq[j-1] + fRq[j-2];
      }

      /*------------------------------------------------------
        This step is actually:

           fRp[1] = fRp[1] + fRp[0]*fDp;

        but fRp[0] = 1.0 always, so the multiplication by
        fRp[0] is omitted.
      ------------------------------------------------------*/
      fRp[1] += fDp;
      fRq[1] += fDq;

      /*------------------------------------------------------
        The final step in this stage of the product is 
          fRp[0] = fRp[0] but since that is redundant it
          is omitted.
      ------------------------------------------------------*/
   }

   /*------------------------------------------------------
     Compute polynomials P(z) and Q(z).  

       P(z) = (1 + z^(-1)) Rp(z)
       Q(z) = (1 - z^(-1)) Rq(z)

       These are computed in place! Again, due to symmetry 
       only half of the coefficients are computed.
   ------------------------------------------------------*/
   for (i=iOrder2; i; i--)
   {
      fRp[i] += fRp[i-1];
      fRq[i] -= fRq[i-1];
   }

   /*------------------------------------------------------
     Compute predictor coefficients:

        A(z) = 0.5 * (P(z) + Q(z))
   ------------------------------------------------------*/
   pfAs[0] = 1.0F;
   for (i=1; i<=iOrder2; i++)
   {
      pfAs[i]          = 0.5F*(fRp[i]+fRq[i]);
      pfAs[iOrder+1-i] = 0.5F*(fRp[i]-fRq[i]);
   }
}


