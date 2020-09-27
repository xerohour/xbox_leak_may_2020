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
* Filename:    CsToEnv.c
*
* Purpose:
*
* Functions:
*
* Author/Date: Bob Dunn 12/27/96
*
*******************************************************************************
*
* Modifications: Wei Wang, cleaned up the code at 12/22/97
*                Wei Wang, Support inter-leave data format
*                Bob Dunn, changed output to inter-leave data format
*
* Comments:      void VoxCsToEnv(const float *pfCs, int iOrderCepstral, 
*                                int iLogLfft, float *pfSW)
*
* Concerns:      
*
* $Header:   G:/pvcsproj/voxlib.prj/CsToEnv.c_v   1.8   03 Mar 1998 09:44:02   weiwang  $
******************************************************************************/
#include <assert.h>
#include <string.h>
#include "vLibTran.h"

#include "xvocver.h"

/* ln(2) to convert cepstral phase to radians */
#define LN_OF_2 0.69314718F

/******************************************************************************
*
* Function:  VoxCsToEnv()
*
* Action:    Compute the cepstral log-amplitude and phase envelopes (using 
*            interleave-formated FFT)
*
* Input:     float *pfCs           --> Cepstral coefficients
*            int iOrderCepstral    --> number of Cepstral coefficients
*            int iLogLfft          --> Log (base 2) of FFT length
*
* Output:    float *pfSW           --> interleaved output array. The real part
*                                          (odd indices) is the Log (base 2) 
*                                          magnitude of spectrum (0 to iLfft/2)
*                                          The imaginary part (even indices)
*                                          is the unwrapped phase (in radians)
*                                          of spectrum (0 to iLfft/2). The 
*                                          length of *pfSW must be 
*                                          (1<<iLogLfft)+2).
*
* Globals:   none
*
* Return:    void
*******************************************************************************
*
* Implementation/Detailed Description:
*
*   NOTE:   The cepstral coefficients have been computed based on
*             the "log_base_2" amplitude data. So to convert the
*             phase to radians it is necessary to multiply the
*             phase envelope by "log_base_e(2)".
* 
*             log_base_2(x) = log_base_e(2)/log_base_2(x) for magnitude
* 
*             log_base_e(x) = log_base_2(x)*log_base_e(2) for phase in radians
*
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
void VoxCsToEnv( const float *pfCs,  
                 int          iOrderCepstral, 
                 int          iLogLfft,     
                 float       *pfSW
               )
{
   int   i, j;
   int   iLfft;

   assert(pfSW != NULL && pfCs != NULL);

   /* FFT size */

   iLfft = (1<<iLogLfft);

   
   /* initialize the memory */

   memset(pfSW, 0, sizeof(float)*(iLfft+2));

   
   /* put cepstral coefficients to buffer before taking real FFT */

   pfSW[0] = pfCs[0];
  
   for(i = 1,j = iLfft - 1;i < iOrderCepstral;i++, j--)
   {
      /*--------------------------------------------------
        This code used to be written as:

           fEvenPart = pfCs[i];
           fOddPart = pfCs[i]*LN_OF_2;
           pfSW[i] = fEvenPart + fOddPart;
           pfSW[j] = fEvenPart - fOddPart;

         which might be better for fixed point.
      --------------------------------------------------*/

      pfSW[i] = pfCs[i] * (1.0F + LN_OF_2);
      pfSW[j] = pfCs[i] * (1.0F - LN_OF_2);
   }

   /* Take the inter-leaved formated real FFT */
   VoxRealFft(pfSW, iLogLfft);
}

