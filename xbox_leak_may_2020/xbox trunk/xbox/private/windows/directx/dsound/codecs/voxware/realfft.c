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
* Filename:     RealFFT.c
*
* Purpose:      FFT and IFFT of real signal with inter-leaved format
*
* Functions:    void RealFft(float *pfInOut, int log2N, int iFlags);
*
*               void RealInverseFft(float *pfInOut, int log2N, int iFlags);
*
* Author/Date:  David Campana, 05/07/97
*
* $Header:   /export/phobos/pvcs/pvcsproj/voxlib.prj/RealFFT.c_v   1.13   03 Mar 1998 11:22:42   weiwang  $
*******************************************************************************/
/*************************************************************************
**
** This is an implementation of the packing/unpacking algorithm for 
** Real FFT / Inverse Real FFT (also known as conjugate symmetrix IFFT).
**
** I am happy with this code because:
**    - pretty fast
**    - pretty clean
**    - uses small twiddle table (1/4 cycle of sine)
**    - uses same format for frequency domain (interleaved) as NSP.
**    - uses stock FFT as part of algorithm. Can be sped up by using
**      a different (ie Radix 4) FFT kernel.
**    - uses no additional memory, and very small stack space.
**
** General Comment:
**    The code uses a mixture of arrays and pointers. On some machines, 
**    the code may run faster if the twiddle index (array) is converted
**    to a pointer. This is pretty easy to do.
**
** Improvements/TBD?
**    Some use of a radix 4 algorithm would be nice. Perhaps, the real FFT/IFFT
**    could call radix 4 kernels where appropriate.
**    Perhaps we could write a radix 4 kernel that had the ability to handle
**    N = 2^k != 4^j.
**
** Author:  David Campana
** Created: May 7, 1997.
**
** Modification:
**         June 24, 1997   Changed to fixpoint by Xiangdong Fu
**         Dec  23, 1997   Took improved fixed point code and converted to floating pt.
**                         Added ComplexFft(), InverseComplexFft() functions 
**                         for completeness and did some general cleanup and commenting.
**         Feb  23, 1997   Added in Wei's changes.
**                         Changed to in place only
**                         Changed variable names to Hungarian.
**
*****************
** Data Format **
*****************
** 
** Real FFT/IFFT:
**
** Time Domain: x[0]  x[1]  x[2]  x[3]  ... x[N-2]    x[N-1]
**
** Frequency:   Xr[0] Xi[0] Xr[1] Xi[1] ... Xr[N/2-1] Xi[N/2-1] Xr[N/2] Xi[N/2]
**
** There are therefore two more frequency domain data points than there
** are for time domain!!! 
** Xi[0] == Xi[N/2] == 0
**
**
**************************************************************************/

#include <math.h>
#include <stdlib.h>
#include <assert.h>
#include "vLibTran.h"

#include "xvocver.h"

/*************
**************
**          **
** Defines  **
**          **
**************
*************/

/*************
**************
**          **
** globals  **
**          **
**************
*************/

static const float twiddle[] =
{
#include "SinTab.h"
};

#define  TWIDDLE_TABLE_SIZE  (sizeof(twiddle)/sizeof(float))
/* if assumes the twiddle table contains 1/4 period of the twiddle table. */
#define  TWIDDLE_PERIOD  ((TWIDDLE_TABLE_SIZE - 1)*4)
#define  STAGE_3_TWIDDLE_INC  (TWIDDLE_PERIOD/8)

const float *sin0Index = twiddle;
const float *cos0Index = (&(twiddle[TWIDDLE_PERIOD/4]));


/****************
*****************
**             **
** Prototypes  **
**             **
*****************
****************/

static void Fft(float *pfInOut, int log2N);

static void Unpack(float *pfInOut, int log2N);

static void InverseUnpack(float *pfInOut, int log2N);

static void InverseFft(float *pfInOut, int log2N);

static void BitRev(float *pfInOut, int N);



/******************************************************************************
*
* Function:  RealFft()
*
* Action:    Fast-conversion of real signal to conjugate-symmetric spectrum.
*            The output is inter-leaved.
*
*            float *pfInOut        --> pfInOut real signal/output complex signal
*            int    log2N          --> the order for FFT
*
* Globals:   none
*
* Return:    void
*******************************************************************************
*
* Implementation/Detailed Description:
*
* References:
*******************************************************************************
*
* Modifications:
*
**          - This is the umbrella function. It calls out the other functions
**          that do the actual work.
**          - Frequency domain signal is interleaved, and has 2 extra locations.
**          See data format discussion above for more info.
**          - "Packing" algorithm, based on manipulating the output of
**          a complex FFT of half the size of the real FFT.
**          Not the most efficient algorithm, but good by most standards.
* Comments:  pfInOut length N and output length N+2.
*
* Concerns/TBD: 
******************************************************************************/

void RealFft(float *pfInOut, int log2N)
{
  int N;
  
  N = 1<<log2N;

  assert(pfInOut != NULL);
  assert(N >= 8);
  assert(N <= TWIDDLE_PERIOD);
  assert(log2N>0);
  
  BitRev(pfInOut, (N>>1));
  
  Fft(pfInOut, (log2N-1));

  Unpack(pfInOut, log2N);
}



/******************************************************************************
*
* Function:  RealInverseFft()
*
* Action:    Fast-conversion of conjugate-symmetric spectrum to real signal.
*            The pfInOut is inter-leaved.
*
*            float *pfInOut        --> pfInOut conjugate-symmetric spectrum
*                                      output real time signal
*            int    log2N          --> the order for FFT
*            int    iFlags         --> controls special functions
*
* Globals:   none
*
* Return:    void
*******************************************************************************
*
* Implementation/Detailed Description:
*
* References:
*******************************************************************************
*
* Modifications:
*
**          - This is the umbrella function. It calls out the other functions
**          that do the actual work.
**          - Frequency domain signal is interleaved, and has 2 extra locations.
**          See data format discussion above for more info.
**          - "Packing" algorithm, based on manipulating the output of
**          a complex FFT of half the size of the real FFT.
**          Not the most efficient algorithm, but good by most standards.
*
* Comments:  pfInOut length N+2 and output length N.
*
* Concerns/TBD: 
******************************************************************************/

void RealInverseFft(float *pfInOut, int log2N, int iFlags)
{
   int   i,
         N,
         N_2;

   float fScale;
   
   N = 1 << log2N;

   assert (pfInOut != NULL);
   assert (N >= 8);
   assert (N <= TWIDDLE_PERIOD);
   assert (log2N >= 3);

   InverseUnpack(pfInOut, log2N);

   N_2 = N>>1;

   BitRev(pfInOut, N_2);
   InverseFft(pfInOut, log2N-1);

   if ((iFlags & IFFT_NO_SCALE_FLAG) == 0)
   {
      /***** then do the scaling *****/
     fScale = 1.0F / (float)N;
     for (i = 0; i < N; i++)
       pfInOut[i] *= fScale;
   }
    
}




/***************************************************************************
**
** Fft
** ---
**
**
** Note:    This is an inplace only algorithm.
**
** Each butterfly iStage looks like ...
**
**
** (n-1)th iStage        nth iStage 
**
** p o-----------o-----o p
**           \  / 
**            \/ 
**            /\ 
**           /  \ 
** q o-- w -- -1 o-----o q
**
** where w is the approriate twiddle factor, and all multiplications
** and additions are complex.
**
** Stages 1 and 2 are calculated separately for speed (twiddle values are
** all -1, 0, +1 for stages 1 and 2).
**
** Stages 3 through log2N are unrolled inside. This allows us to use the
** smaller twiddle table, and saves a few multiplications.
** Woops! We don't do this (as much) anymore, which is fine for floating point,
** but you may still want to do it for fixed point.
**
** This is a radix 2 algorithm. Radix 4 algorithms generally require fewer
** multiplications and are faster (but are limited to powers of 4).
**
**************************************************************************/

static void Fft(float *pfInOut, int log2N)
{

  float        *pfBase,
               *pfPReal,
               *pfPImag,
               *pfQReal,
               *pfQImag;

  const float  *pfWReal,
               *pfWImag;

  int          iWInc,
               iWIndex,
               iButterflySep,
               iGroupsPerStage;
  int          i,
               iStage,
               j;
  float        a, b, c, d;
  int          N;
  
  N = 1<<log2N;

   /*
   ** The first couple of stages I can save significant complexity by
   ** taking advantage of the fact that pfWReal[iWIndex] and pfWImag[iWIndex]
   ** have a lot of nice values for the first couple stages (0, +1, -1)
   */

  assert(pfInOut != NULL);
  assert(N >= 4);
  assert(N <= TWIDDLE_PERIOD/2);
  assert(log2N >= 2);
  
   /************
   *************
   **         **
   ** Stage 1 **
   **         **
   *************
   ************/
   pfBase = pfInOut;
   for (i = (N>>1); i > 0; i--)
   {
      pfPReal = pfBase;
      pfPImag = pfPReal + 1;
      pfQReal = pfPReal + 2;
      pfQImag = pfQReal + 1;

      /* Calculate cross terms in butterfly (W * Q) */

      a = (*pfPReal) - (*pfQReal);
      b = (*pfPReal) + (*pfQReal);
      c = (*pfPImag) - (*pfQImag);
      d = (*pfPImag) + (*pfQImag);

      *pfQReal = a;
      *pfPReal = b;
      *pfQImag = c;
      *pfPImag = d;

      pfBase += 4;  /* Move ahead to next butterfly */
   }


   /************
   *************
   **         **
   ** Stage 2 **
   **         **
   *************
   ************/
   pfBase = pfInOut;
   for (i = (N>>2); i > 0; i--)
   {
      pfPReal = pfBase;
      pfPImag = pfPReal + 1;
      pfQReal = pfPReal + 4;
      pfQImag = pfQReal + 1;

      /* Calculate cross terms in butterfly (W * Q) */
      a = (*pfPReal) - (*pfQReal); 
      b = (*pfPReal) + (*pfQReal);     
      c = (*pfPImag) - (*pfQImag);       
      d = (*pfPImag) + (*pfQImag); 

      *pfQReal = a; 
      *pfPReal = b;
      *pfQImag = c;
      *pfPImag = d;

      pfBase += 2;  /* Move ahead to next butterfly */

      pfPReal = pfBase;
      pfPImag = pfPReal + 1;
      pfQReal = pfPReal + 4;
      pfQImag = pfQReal + 1;

      /* Calculate cross terms in butterfly (W * Q) */
      a = (*pfPReal) - (*pfQImag);
      b = (*pfPReal) + (*pfQImag);
      c = (*pfPImag) + (*pfQReal);
      d = (*pfPImag) - (*pfQReal);

      *pfQReal = a;
      *pfPReal = b;
      *pfQImag = c;
      *pfPImag = d;

      pfBase += 6;  /* Move ahead to next group */
   }


   /*********************
   **********************
   **                  **
   ** Stage 3 to log2N **
   **                  **
   **********************
   *********************/
   iWInc = STAGE_3_TWIDDLE_INC;
   iButterflySep = 4;
   iGroupsPerStage = N>>3;
   pfWReal = cos0Index;
   pfWImag = sin0Index;
   iWIndex = 0;

   for (iStage = 3; iStage <= log2N; iStage++)
   {
      pfBase = pfInOut;
      for (j = 0; j < iGroupsPerStage; j++)
      {
         for (i = (iButterflySep>>1); i > 0; i--)
         {
            pfPReal = pfBase;
            pfPImag = pfPReal + 1;
            pfQReal = pfPReal + (iButterflySep<<1);
            pfQImag = pfQReal + 1;

            /* Calculate cross terms in butterfly (W * Q) */            
            a = (*pfQReal) * pfWReal[-iWIndex]; 
            b = (*pfQReal) * pfWImag[iWIndex];
            
            a += (*pfQImag) * pfWImag[iWIndex];
            b -= (*pfQImag) * pfWReal[-iWIndex];
           
            c = (*pfPReal) - a;             /* c==>*pfQReal */
            a = (*pfPReal) + a;             /* a==>*pfPReal */

            d = (*pfPImag) + b;             /* d==>*pfQImag */
            b = (*pfPImag) - b;             /* b==>*pfPImag */

            *pfQReal = c;
            *pfPReal = a;
            *pfQImag = d;
            *pfPImag = b;

            /* Modulo addressing */
            assert(iWIndex <= TWIDDLE_TABLE_SIZE);
            iWIndex = (iWIndex + iWInc);
            pfBase += 2;  /* Move ahead to next butterfly */
         }

         for (i = (iButterflySep>>1); i > 0; i--)
         {
            pfPReal = pfBase;
            pfPImag = pfPReal + 1;
            pfQReal = pfPReal + (iButterflySep<<1);
            pfQImag = pfQReal + 1;

            /* Calculate cross terms in butterfly (W * Q) */
            a = (*pfQReal) * pfWReal[-iWIndex]; 
            b = (*pfQReal) * pfWImag[iWIndex];
            
            a -= (*pfQImag) * pfWImag[iWIndex];
            b += (*pfQImag) * pfWReal[-iWIndex];
           
            c = (*pfPReal) + a;       /* c==>*pfQReal */
            a = (*pfPReal) - a;       /* a==>*pfPReal */
            d = (*pfPImag) + b;       /* d==>*pfQImag */
            b = (*pfPImag) - b;       /* b==>*pfPImag */

            (*pfQReal) = c;
            (*pfPReal) = a;
            (*pfQImag) = d;
            (*pfPImag) = b;

            /* Modulo addressing */
            assert(iWIndex <= TWIDDLE_TABLE_SIZE);
            assert(iWIndex >= 0);
            iWIndex = (iWIndex - iWInc);
            pfBase += 2;  /* Move ahead to next butterfly */
         }

        /* Prepare for next group in current iStage */
         pfBase += (iButterflySep<<1);
      }

      /* Prepare for next iStage */
      iButterflySep <<= 1;          /* iButterflySep *= 2   */
      iWInc >>= 1;                  /* iWInc /= 2           */
      iGroupsPerStage >>= 1;        /* iGroupsPerStage /= 2 */
   }
}






/***************************************************************************
**
** InverseFft
** ----------
**
**
** Note:    This is an inplace algorithm.
**
** Each butterfly iStage looks like ...
**
**
** (n-1)th iStage        nth iStage 
**
** p o-----------o-----o p
**           \  / 
**            \/ 
**            /\ 
**           /  \ 
** q o-- w -- -1 o-----o q
**
** where w is the approriate twiddle factor, and all multiplications
** and additions are complex.
**
** Stages 1 and 2 are calculated separately for speed (twiddle values are
** all -1, 0, +1 for stages 1 and 2).
**
** Stages 3 through log2N are unrolled inside. This allows us to use the
** smaller twiddle table, and saves a few multiplications.
** Woops! We don't do this (as much) anymore, which is fine for floating point,
** but you may still want to do it for fixed point.
**
** This is a radix 2 algorithm. Radix 4 algorithms generally require fewer
** multiplications and are faster (but are limited to powers of 4).

***************************************************************************/

static void InverseFft(float *pfInOut, int log2N)
{
   float       *pfBase,
               *pfPReal,
               *pfPImag,
               *pfQReal,
               *pfQImag;

   float       a, b, c, d;

   const float *pfWReal,
               *pfWImag;

   int         iWInc,
               iWIndex,
               iButterflySep,
               iGroupsPerStage;

   int         i,
               iStage,
               j;

   int         N;

   N = 1<<log2N;

   assert(pfInOut != NULL);
   assert(N >= 4);
   assert(N <= TWIDDLE_PERIOD);
   assert(log2N >= 2);

   /*
   ** The first couple of stages I can save significant complexity by
   ** taking advantage of the fact that pfWReal[iWIndex] and pfWImag[iWIndex]
   ** have a lot of nice values for the first couple stages (0, +1, -1)
   */

   /************
   *************
   **         **
   ** Stage 1 **
   **         **
   *************
   ************/
   pfBase = pfInOut;
   for (i = 0; i < (N>>1); i++)
   {
      pfPReal = pfBase;
      pfPImag = pfPReal + 1;
      pfQReal = pfPReal + 2;
      pfQImag = pfQReal + 1;

      a = (*pfPReal) - (*pfQReal);
      b = (*pfPReal) + (*pfQReal);
      c = (*pfPImag) - (*pfQImag);
      d = (*pfPImag) + (*pfQImag);

      *pfQReal = a;
      *pfPReal = b;
      *pfQImag = c;
      *pfPImag = d;
      pfBase += 4;  /* Move ahead to next butterfly */
   }
  
   /************
   *************
   **         **
   ** Stage 2 **
   **         **
   *************
   ************/
   pfBase = pfInOut;
   for (i = 0; i < (N>>2); i++)
   {
      pfPReal = pfBase;
      pfPImag = pfPReal + 1;
      pfQReal = pfPReal + 4;
      pfQImag = pfQReal + 1;

      /* Calculate cross terms in butterfly (W * Q) */    
      a = (*pfPReal) - (*pfQReal); 
      b = (*pfPReal) + (*pfQReal);     
      c = (*pfPImag) - (*pfQImag);       
      d = (*pfPImag) + (*pfQImag); 

      (*pfQReal) = a;
      (*pfPReal) = b;
      (*pfQImag) = c;
      (*pfPImag) = d;
      
      pfBase += 2;  /* Move ahead to next butterfly */

      pfPReal = pfBase;
      pfPImag = pfPReal + 1;
      pfQReal = pfPReal + 4;
      pfQImag = pfQReal + 1;

      /* Calculate cross terms in butterfly (W * Q) */


      a = (*pfPReal) + (*pfQImag);
      b = (*pfPReal) - (*pfQImag);
      c = (*pfPImag) - (*pfQReal);
      d = (*pfPImag) + (*pfQReal);

      *pfQReal = a;
      *pfPReal = b;
      *pfQImag = c;
      *pfPImag = d;

      pfBase += 6;  /* Move ahead to next group */

   }
   
   /*********************
   **********************
   **                  **
   ** Stage 3 to log2N **
   **                  **
   **********************
   *********************/

   iWInc = STAGE_3_TWIDDLE_INC;
   iButterflySep = 4;
   iGroupsPerStage = N >> 3;
   iWIndex = 0;
   pfWReal = cos0Index;
   pfWImag = sin0Index;
   
   for (iStage = 3; iStage <= log2N; iStage++)
   {
      pfBase = pfInOut;
      for (j = 0; j < iGroupsPerStage; j++)
      {
         for (i = (iButterflySep>>1); i > 0; i--)
         {
            pfPReal = pfBase;
            pfPImag = pfPReal + 1;
            pfQReal = pfPReal + (iButterflySep<<1);
            pfQImag = pfQReal + 1;

            /* Calculate cross terms in butterfly (W * Q) */
            a = (*pfQReal) * pfWReal[-iWIndex]; 
            b = (*pfQReal) * pfWImag[iWIndex];
            
            a -= (*pfQImag) * pfWImag[iWIndex];
            b += (*pfQImag) * pfWReal[-iWIndex];
           
            /* convert data from Q15 to Q30 format before 
            addition/substraction */
            c = (*pfPReal) - a;    /* c==>*pfQReal */
            a = (*pfPReal) + a;    /* a==>*pfPReal */
            d = (*pfPImag) - b;    /* d==>*pfQImag */
            b = (*pfPImag) + b;    /* b==>*pfPImag */

            *pfQReal = c;
            *pfPReal = a;
            *pfQImag = d;
            *pfPImag = b;

            assert(iWIndex <= TWIDDLE_TABLE_SIZE);
            iWIndex = iWIndex + iWInc;
            pfBase += 2;  /* Move ahead to next butterfly */
         }
        
         for (i = (iButterflySep>>1); i > 0; i--)
         {
            assert(pfBase <= (pfInOut + (N << 1)));
            pfPReal = pfBase;
            pfPImag = pfPReal + 1;
            pfQReal = pfPReal + (iButterflySep<<1);
            pfQImag = pfQReal + 1;

            /* Calculate cross terms in butterfly (W * Q) */
            a = (*pfQReal) * pfWReal[-iWIndex]; 
            b = (*pfQReal) * pfWImag[iWIndex];
            
            a += (*pfQImag) * pfWImag[iWIndex];
            b -= (*pfQImag) * pfWReal[-iWIndex];
           
            c = (*pfPReal) + a;       /* c==>*pfQReal */
            a = (*pfPReal) - a;       /* a==>*pfPReal */
            d = (*pfPImag) - b;       /* d==>*pfQImag */
            b = (*pfPImag) + b;       /* b==>*pfPImag */

            *pfQReal = c;
            *pfPReal = a;
            *pfQImag = d;
            *pfPImag = b;

            assert(iWIndex <= TWIDDLE_TABLE_SIZE);
            assert(iWIndex >= 0);
            iWIndex = iWIndex - iWInc;
            pfBase += 2;  /* Move ahead to next butterfly */
         }
         pfBase += (iButterflySep<<1);
      }
      iButterflySep <<= 1;
      iWInc >>= 1;
      iGroupsPerStage >>= 1;
   }
}








/*************************************************************************
**
** Unpack
** ------
**
** Converts the output of the complex FFT into 
** a real FFT output.
**
** First iStage is to form the realEven, realOdd, imagEven, and
** imagOdd components of the FFT output.
** The second iStage is to combine these components in a 
** "butterfly" operation to form the real FFT output.
**
*************************************************************************/
static void Unpack(float *pfInOut, int log2N)
{
   int         k;
   const float *pfCos,
               *pfSin;
   int         twiddleMod;
   float       a, b, c, d;
   float       x0, x1, y0, y1;
   int         N;
   
   N = 1<<log2N;

   assert(pfInOut != NULL);
   assert(N >= 8);
   assert(N <= TWIDDLE_PERIOD);

   /* Define the boundary conditions */
   /* The 0 and 1 terms and the N and N+1 terms */
   a = pfInOut[0] - pfInOut[1];
   b = pfInOut[0] + pfInOut[1];
   
   pfInOut[N] = a;
   pfInOut[0] = b;

   pfInOut[N+1] = 0.0F;
   pfInOut[1] = 0.0F;

   pfCos = cos0Index;
   pfSin = sin0Index;
   twiddleMod = TWIDDLE_PERIOD >> log2N;

   for (k = 2; k < (N>>1); k+=2)
   {
      x0 = 0.5f * (pfInOut[k]   + pfInOut[N-k]);
      x1 = 0.5f * (pfInOut[k]   - pfInOut[N-k]);
      y0 = 0.5f * (pfInOut[k+1] + pfInOut[N-k+1]); 
      y1 = 0.5f * (pfInOut[k+1] - pfInOut[N-k+1]);     

      pfCos -= twiddleMod;
      pfSin += twiddleMod;

      a =  (*pfCos) * y0;
      b =  (*pfCos) * x1;
      a -= (*pfSin) * x1;      /* a = Wr*IE-Wi*RO  */
      b += (*pfSin) * y0;      /* b = Wr*RO+Wi*IE  */

      c = x0 + a;             /* pfInOut[k]         */
      d = x0 - a;             /* pfInOut[N-k]       */
      a = y1 - b;             /* pfInOut[k+1]       */
      b = y1 + b;             /* -pfInOut[N-k+1]    */

      pfInOut[k]         =  c; 
      pfInOut[N - k]     =  d;
      pfInOut[k + 1]     =  a;
      pfInOut[N - k + 1] =  -b;
   }

   /* The N/2 term */
   /* at this point k = N/2 */
   /* pfInOut[k] = pfInOut[k]; */
   pfInOut[k + 1] = -pfInOut[k+1];
}






/*************************************************************************
**
** InverseUnpack
** --------------
**
** Converts the complex FFT values into a packed sequence,
** after which the FFT is performed.
**
** First iStage is to form the realEven, realOdd, imagEven, and
** imagOdd components of the FFT output.
** The second iStage is to combine these components in a 
** "butterfly" operation to form another sequence.
** The output sequence will be used as an pfInOut to a size N IFFT.
**
** This function conjugates the imaginary
** components as it goes along.
**
**
*************************************************************************/
static void InverseUnpack(float *pfInOut, int log2N)
{
   int         k;
   const float *pfCos,
               *pfSin;
   int         twiddleMod;
   float       a, b, c, d;
   float       x0, x1, y0, y1;
   int         N;
   int         Nv2;

   N = 1<<log2N;

   assert(pfInOut != NULL);
   assert(N >= 8);
   assert(N <= TWIDDLE_PERIOD);
   assert(log2N >= 3);

   /* Define boundary conditions */

   /* the 0th and 1st terms */
   a =  pfInOut[0] + pfInOut[N];   /* realEven   */
   b =  pfInOut[1] + pfInOut[N+1]; /* imagEven   */
   c =  pfInOut[0] - pfInOut[N];   /* realOdd    */
   d =  pfInOut[1] - pfInOut[N+1]; /* imagOdd    */

   c = c + d;
   a = a - b;

   pfInOut[0] = a;
   pfInOut[1] = c;

   /* The N/2 term */
   Nv2 = N>>1;

   pfInOut[Nv2]   =  2.0f * pfInOut[Nv2];
   pfInOut[Nv2+1] = -2.0f * pfInOut[Nv2+1];

   /* second iStage - combine in "butterfly" fashion" */
   pfCos = cos0Index;
   pfSin = sin0Index;
   twiddleMod = TWIDDLE_PERIOD >> log2N;

   for (k=2; k < Nv2; k+=2)
   {
      x0 = pfInOut[k]   + pfInOut[N-k];
      x1 = pfInOut[k]   - pfInOut[N-k];
      y0 = pfInOut[k+1] + pfInOut[N-k+1]; 
      y1 = pfInOut[k+1] - pfInOut[N-k+1];     

      pfCos -= twiddleMod;
      pfSin += twiddleMod;

      a =  (*pfCos) * y0;
      b =  (*pfCos) * x1;
      a += (*pfSin) * x1;   /* a = -(Wr*IE-Wi*RO) */
      b -= (*pfSin) * y0;   /* b = Wr*RO+Wi*IE    */

      c = x0 - a;          /* output[k]      */
      d = x0 + a;          /* output[N-k]    */
      a = y1 + b;          /* output[k+1]    */
      b = y1 - b;          /* -output[N-k+1] */
    
      pfInOut[k]           = c; 
      pfInOut[N - k]       = d;
      pfInOut[k + 1]       = a;
      pfInOut[N - k + 1]   = -b;
   }
}


   





/*************************************************************************
**
** BitRev
** ------
**
** Interprets the data as an interleaved complex array and bit reverses
** the data in place.
**
** The total effect of the packing and bit reversal process
** is shown in the following example
**
**
** pfInOut             Packed         Bit Reversed
** -----             ------         ------------
** x[000]            r[00]          r[00]
** x[001]            i[00]          i[00]
** x[010]            r[01]          r[10] 
** x[011]   -->      i[01]    -->   i[10]
** x[100]            r[10]          r[01]
** x[101]            i[10]          i[01] 
** x[110]            r[11]          r[11]
** x[111]            i[11]          i[11]
**
** Note:
** The pfInOut N is the number of _complex_ elements in the output
** sequence. Therefore, the function expects 2N realvalued pfInOut
** elements.
**
** This version compes from Numerical Recipes (page 507)
**
*************************************************************************/
static void BitRev(float *pfInOut, int N)
{
   int   i, j, n, m;
   float temp;

   /* This algorithm is for in place manipulation */
   
   n = (N << 1);
   j = 1;
   for (i = 1; i < n; i +=2)
   {
      if (j > i)
      {
         /* swap i and j (imaginary part) */
         temp = pfInOut[j];
         pfInOut[j] = pfInOut[i];
         pfInOut[i] = temp;
         
         /* swap (i - 1) and (j - 1) (real part) */
         temp = pfInOut[j-1];
         pfInOut[j-1] = pfInOut[i-1];
         pfInOut[i-1] = temp;
      }
      m = (n >> 1);
      while (m >= 2 && j > m)
      {
         j = j - m;
         m >>= 1;
      }
      j = j + m;
   }
}
